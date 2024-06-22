/* Copyright 2024 Maxim Iorsh <iorsh@users.sourceforge.net>

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

The name of the author may not be used to endorse or promote
products derived from this software without specific prior written
permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "common_menus.hpp"

#include <filesystem>
#include <iostream>
#include <glib/gi18n.h>

#include "font_view.hpp"
#include "menu_ids.h"

using FontViewNS::FontViewUiContext;

namespace FF {

std::vector<PythonMenuItem> python_menu_items;

void register_py_menu_item(const PyMenuSpec* spec, int flags) {
    FF::PythonMenuItem py_menu_item;

    py_menu_item.flags = flags;
    py_menu_item.divider = spec->divider;
    py_menu_item.shortcut = spec->shortcut_str ? spec->shortcut_str : "";
    py_menu_item.func = spec->func;
    py_menu_item.check = spec->check;
    py_menu_item.data = spec->data;

    py_menu_item.levels.resize(spec->depth);
    for (size_t i = 0; i < spec->depth; ++i) {
	const auto& level = spec->levels[i];
	py_menu_item.levels[i].localized = level.localized ? level.localized : "";
	py_menu_item.levels[i].untranslated = level.untranslated ? level.untranslated : "";
	py_menu_item.levels[i].identifier = level.identifier ? level.identifier : "";
    }

    FF::python_menu_items.push_back(std::move(py_menu_item));
}

static std::vector<FF::MenuInfo>::iterator
add_or_update_item(std::vector<FF::MenuInfo>& menu, const std::string& label) {
    // Try to find an existing menu item by the localized label.
    auto iter = std::find_if(menu.begin(), menu.end(),
    	[label](const FF::MenuInfo& mi){ return (Glib::ustring)mi.label.text == label; });

    if (iter == menu.end()) {
	// Label not found, create an empty item for it
	FF::MenuInfo new_item{ { label.c_str(), FF::NonCheckable, "" }, nullptr, FF::SubMenuCallbacks, 0 };
	menu.push_back(new_item);
	iter = std::prev(menu.end());
    }

    return iter;
}

// Reproduce menu building logic from InsertSubMenus() with the following omissions:
//
// * Mnemonics are ignored - GTK should take care of them.
// * Custom hotkeys are currently not supported.
std::vector<FF::MenuInfo> python_tools(const FF::UiContext& ui_context) {
    const FontViewUiContext& fv_ui_context = static_cast<const FontViewUiContext&>(ui_context);
    FVContext* fv_context = fv_ui_context.get_legacy_context();
    std::vector<FF::MenuInfo> tools_menu;

    for (const auto& py_item : python_menu_items) {
	// Track the target submenu for the current menuitem.
	auto menu_ptr = &tools_menu;

	size_t submenu_depth = py_item.levels.size() - 1;
	for (size_t i=0; i<submenu_depth; ++i) {
	    // Find or create submenu among existing items.
	    auto menu_iter = add_or_update_item(*menu_ptr, py_item.levels[i].localized);

	    // Create submenu list if necessary and descend to it.
	    if (!menu_iter->sub_menu) {
		menu_iter->sub_menu = new std::vector<FF::MenuInfo>();
	    }
	    menu_ptr = menu_iter->sub_menu;
	}

	if (py_item.divider) {
	    menu_ptr->push_back(kMenuSeparator);
	} else {
	    auto menu_iter = add_or_update_item(*menu_ptr, py_item.levels.back().localized);

	    // Define the new menu item. If it was already present, it will be redefined.
	    if (py_item.check) {
	    	menu_iter->callbacks.enabled =
	            [fv_context, py_item](const FF::UiContext&)
		    { return fv_context->py_check(fv_context->fv, py_item.levels.back().localized.c_str(), py_item.check, py_item.data); };
	    }

	    menu_iter->callbacks.handler =
	        [fv_context, py_item](const FF::UiContext&)
		{ return fv_context->py_activate(fv_context->fv, py_item.func, py_item.data); };
	}
    }

    return tools_menu;
}

std::vector<FF::MenuInfo> recent_files(const FF::UiContext& ui_context) {
    const FontViewUiContext& fv_ui_context = static_cast<const FontViewUiContext&>(ui_context);
    FVContext* fv_context = fv_ui_context.get_legacy_context();
    char** recent_files_array = nullptr;
    int n_recent = fv_context->collect_recent_files(&recent_files_array);
    std::vector<FF::MenuInfo> info_arr;

    for (int i = 0; i < n_recent; ++i) {
	// Pointer to persistent memory
        const char* file_path = recent_files_array[i];

        FF::ActivateCB action = [show_font = fv_context->show_font, file_path](const FF::UiContext&){
            show_font(file_path, 0);
        };
	std::string label = std::filesystem::path(file_path).filename().string();

        FF::MenuInfo info{ { label.c_str(), FF::NonCheckable, "" }, nullptr, { action, FF::AlwaysEnabled, FF::NotCheckable }, 0 };
        info_arr.push_back(info);
    }
    return info_arr;
}

std::vector<FF::MenuInfo> legacy_scripts(const FF::UiContext& ui_context) {
    const FontViewUiContext& fv_ui_context = static_cast<const FontViewUiContext&>(ui_context);
    FVContext* fv_context = fv_ui_context.get_legacy_context();
    char** script_names_array = nullptr;
    int n_scripts = fv_context->collect_script_names(&script_names_array);
    std::vector<FF::MenuInfo> info_arr;

    FVMenuAction* callback_set = FF::find_legacy_callback_set(MID_ScriptMenu, fv_context->actions);
    void (*c_action)(FontView*, int) = callback_set->action;
    FontView* fv = fv_context->fv;

    for (int i = 0; i < n_scripts; ++i) {
	FF::ActivateCB action = [c_action, fv, i](const UiContext&){ c_action(fv, i); };
	Glib::ustring accel = "<alt><control>" + std::to_string((i + 1) % 10);

        FF::MenuInfo info{ { script_names_array[i], FF::NonCheckable, accel }, nullptr, { action, FF::AlwaysEnabled, FF::NotCheckable }, i };
        info_arr.push_back(info);
    }
    return info_arr;
}

static Glib::ustring get_window_title(FVContext* context, const TopLevelWindow* top_win) {
    return top_win->is_gtk ? ((Gtk::Window*)top_win->window)->get_title() : context->get_window_title((GWindow)(top_win->window));
}
static void raise_window(FVContext* context, const TopLevelWindow* top_win) {
    top_win->is_gtk ? ((Gtk::Window*)top_win->window)->raise() : context->raise_window((GWindow)(top_win->window));
}

std::vector<FF::MenuInfo> top_windows_list(const FF::UiContext& ui_context) {
    const FontViewUiContext& fv_ui_context = static_cast<const FontViewUiContext&>(ui_context);
    FVContext* fv_context = fv_ui_context.get_legacy_context();
    TopLevelWindow* top_level_windows = nullptr;
    int n_windows = fv_context->collect_windows(&top_level_windows);
    std::vector<FF::MenuInfo> info_arr;

    for (int i = 0; i < n_windows; ++i) {
	FF::ActivateCB action = [fv_context, win_ptr = top_level_windows + i](const UiContext&){ raise_window(fv_context, win_ptr); };
	Glib::ustring title = get_window_title(fv_context, top_level_windows + i);

        FF::MenuInfo info{ { title.c_str(), FF::NonCheckable, "" }, nullptr, { action, FF::AlwaysEnabled, FF::NotCheckable }, 0 };
        info_arr.push_back(info);
    }
    return info_arr;
}

}
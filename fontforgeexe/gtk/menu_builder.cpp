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

#include "menu_builder.hpp"

#include <iostream>

namespace FF {

Gtk::RadioButtonGroup& get_grouper(RadioGroup g) {
    static std::map<RadioGroup, Gtk::RadioButtonGroup> grouper_map;
    static std::map<RadioGroup, Gtk::RadioMenuItem> dummy_item_map;

    auto& grouper = grouper_map[g];

    // Sometimes none of the radio group items should be checked. GTK doesn't have this capability,
    // so we create a predefined dummy item which absorbs the checked state when no other real
    // item wants to be checked.
    if (!dummy_item_map.count(g)) {
        dummy_item_map[g] = Gtk::RadioMenuItem(grouper, "dummy", false);
    }

    return grouper;
}

std::vector<FF::MenuInfo> expand_custom_blocks(const std::vector<FF::MenuInfo>& info, const UiContext& ui_context) {
   std::vector<FF::MenuInfo> expanded_info;
   for (const auto& item : info) {
      if (item.is_custom_block()) {
         std::vector<FF::MenuInfo> block = item.callbacks.custom_block(ui_context);
         expanded_info.insert(expanded_info.end(), block.begin(), block.end());
      } else {
         expanded_info.push_back(item);
      }
   }
   return expanded_info;
}

Gtk::MenuItem* menu_item_factory(const FF::MenuInfo& item,
				 const UiContext& ui_context,
				 int icon_height) {
    Gtk::MenuItem* menu_item = nullptr;
    if (item.is_separator()) {
        menu_item = new Gtk::SeparatorMenuItem();
    } else if (item.label.decoration.empty()) {
        menu_item = new Gtk::MenuItem(item.label.text, true);
    } else if (item.label.decoration.comment()) {
        menu_item = new Gtk::MenuItem(item.label.text);

	// Italic text
	Gtk::Label* menu_label = (Gtk::Label*)menu_item->get_child();
        Glib::ustring text = Glib::ustring::compose("<i>%1</i>", item.label.text);
	menu_label->set_markup(text);
    } else if (item.label.decoration.has_group()) {
        RadioGroup group = item.label.decoration.group();
        Gtk::RadioButtonGroup& grouper = get_grouper(group);
        menu_item = new Gtk::RadioMenuItem(grouper, item.label.text, true);
    } else if (item.label.decoration.checkable()) {
        menu_item = new Gtk::CheckMenuItem(item.label.text, true);
    } else if (item.label.decoration.named_icon()) {
        Glib::RefPtr<Gdk::Pixbuf> pixbuf = load_icon(item.label.decoration.image_file(), icon_height);
        Gtk::Image* img = new Gtk::Image(pixbuf);
        menu_item = new Gtk::ImageMenuItem(*img, item.label.text, true);
    } else {
        Glib::RefPtr<Gdk::Pixbuf> pixbuf = build_color_icon(item.label.decoration.color(), icon_height);
        Gtk::Image* img = new Gtk::Image(pixbuf);
        menu_item = new Gtk::ImageMenuItem(*img, item.label.text, true);
    }

    if (!item.label.accelerator.empty()) {
	Gtk::AccelKey key(item.label.accelerator);
	menu_item->add_accelerator("activate", ui_context.get_accel_group(),
	                           key.get_key(), key.get_mod(), Gtk::ACCEL_VISIBLE);
    }

    if (item.sub_menu) {
        Gtk::Menu* submenu = place_dynamic_menu(*item.sub_menu, ui_context);
        menu_item->set_submenu(*submenu);
    }

    ActivateCB handler = item.callbacks.handler ? item.callbacks.handler : ui_context.get_activate_cb(item.mid);
    std::function<void(void)> action;

    Gtk::CheckMenuItem* check_menu_item = dynamic_cast<Gtk::CheckMenuItem*>(menu_item);
    if (check_menu_item) {
	// For radio items the action is triggered both when the item gains and loses selection.
	// The radio item which just lost its selection doesn't need to activate its callback.
        action = [check_menu_item, handler, &ui_context](){
            if (check_menu_item->get_active()) {
                handler(ui_context);
            }
        };
    } else {
        action = [handler, &ui_context](){ handler(ui_context); };
    }
    menu_item->signal_activate().connect(action);

    return menu_item;
}

Gtk::MenuItem* get_menu_item(const FF::MenuInfo& item,
			     const UiContext& ui_context,
			     int icon_height) {
    const std::string& cache_key = item.label.accelerator;

    // Reuse menu items which have keyboard shortcuts
    if (!cache_key.empty() && ui_context.menu_cache.count(cache_key)) {
	return ui_context.menu_cache.at(cache_key);
    }

    Gtk::MenuItem* menu_item = menu_item_factory(item, ui_context, icon_height);

    if (!cache_key.empty()) {
	ui_context.menu_cache[cache_key] = menu_item;
    }
    return menu_item;
}

void build_sub_menu(Gtk::Menu* menu, const std::vector<FF::MenuInfo>& info, const UiContext& ui_context) {
   Gtk::Widget* bar = gtk_find_child(ui_context.window_, "TopBar");
   int icon_height = std::max(16, bar->get_allocated_height() / 2);

   // If the menu contains custom block, we expand it before further processing
   std::vector<FF::MenuInfo> expanded_menu;
   bool has_custom_blocks = std::find_if(info.begin(), info.end(),
       [](const auto& item){ return item.is_custom_block(); }) != info.end();
   if (has_custom_blocks) {
      expanded_menu = expand_custom_blocks(info, ui_context);
   }
   const std::vector<FF::MenuInfo>& local_info = has_custom_blocks ? expanded_menu : info;

   // Clear existing menu items
   menu->foreach([menu](Gtk::Widget& w){ menu->remove(w); });

   for (const auto& item : local_info) {
      Gtk::MenuItem* menu_item = get_menu_item(item, ui_context, icon_height);

      // Set enabled / disabled state from callback result
      EnabledCB enabled_check =
          item.label.decoration.comment() ? NeverEnabled
                                          : item.callbacks.enabled ? item.callbacks.enabled
					                           : ui_context.get_enabled_cb(item.mid);
      menu_item->set_sensitive(enabled_check(ui_context));

      Gtk::CheckMenuItem* check_menu_item = dynamic_cast<Gtk::CheckMenuItem*>(menu_item);
      if (check_menu_item) {
         CheckedCB checked_cb = item.callbacks.checked ? item.callbacks.checked : ui_context.get_checked_cb(item.mid);

	 // Gtk::Widget::set_state_flags() allows us to set visual item state without triggering its action.
	 check_menu_item->set_state_flags(checked_cb(ui_context) ? Gtk::STATE_FLAG_CHECKED
	 							 : Gtk::STATE_FLAG_NORMAL);
      }

      menu->append(*menu_item);
      menu_item->show();
   }
}

// Callable class which dynamically builds sub-menu
class SubmenuBuilderCB {
public:
   SubmenuBuilderCB(Gtk::Menu* sub_menu, const std::vector<FF::MenuInfo>& info, const UiContext& ui_context)
   : sub_menu_(sub_menu), info_(info), ui_context_(ui_context) {}

   void operator()() {
        build_sub_menu(sub_menu_, info_, ui_context_);
   }
private:
   Gtk::Menu* sub_menu_;
   std::vector<FF::MenuInfo> info_;
   const UiContext& ui_context_;
};

// Top window menus and submenus are dynamic (the number of items and their properties can change on-fly).
// With the creation of a menu item which invokes a submenu, its submenu is not actually created.
// We only create a placeholder with a callback which would be invoked just before the required menu is
// actually shown. When invoked, the callback fills the entire contents of the placeholder according
// to the current state.
Gtk::Menu* place_dynamic_menu(const std::vector<FF::MenuInfo>& info, const UiContext& ui_context) {
   Gtk::Menu* menu = new Gtk::Menu();

   SubmenuBuilderCB submenu_builder(menu, info, ui_context);
   menu->signal_show().connect(submenu_builder);

   // Enable all menu items when the menu is hidden, to ensure that keyboard shortcuts can be
   // always activated.
   auto on_menu_hide = [menu](){
        menu->foreach([](Gtk::Widget& w){ w.set_sensitive(true); });
   };
   menu->signal_hide().connect(on_menu_hide);

   return menu;
}

Gtk::MenuBar* build_menu_bar(const std::vector<FF::MenuBarInfo>& info, const UiContext& ui_context) {
   Gtk::MenuBar* menu_bar = new Gtk::MenuBar();

   for (const auto& item : info) {
      Gtk::MenuItem* menu_item = new Gtk::MenuItem(item.label.text, true);
      menu_bar->append(*menu_item);

      if (item.sub_menu) {
         Gtk::Menu* sub_menu = place_dynamic_menu(*item.sub_menu, ui_context);
         menu_item->set_submenu(*sub_menu);
      }
   }

   return menu_bar;
}

}
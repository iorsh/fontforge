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

#pragma once

#include <variant>
#include <gtkmm-3.0/gtkmm.h>

#include "ui_context.hpp"
#include "utils.hpp"

namespace FF {

enum BasicState {
    NonCheckable,
    Checkable,
    Comment,
};

enum RadioGroup {
    NoGroup,
    CellWindowSize,
    CellPixelView,
    ActiveLayer,
    GlyphLabel,
    Encoding,
    ForcedEncoding,
};

// Lazily initialized collection of GTK groupers for radio buttons
// TODO: get grouper by group id and window id. Current implementation
// would break if there is more than one window.
Gtk::RadioButtonGroup& get_grouper(RadioGroup g);

Gtk::RadioMenuItem& get_dummy_radio_item(RadioGroup g);

class LabelDecoration {
public:
        LabelDecoration(BasicState s = NonCheckable) : d_(s) {}
        LabelDecoration(const char* image_file) : d_(image_file) {}
        LabelDecoration(RadioGroup g) : d_(g) {}
        LabelDecoration(Gdk::RGBA c) : d_(c) {}

        bool empty() const { return d_.index() == 0 && std::get<0>(d_) == NonCheckable; }
        bool checkable() const { return d_.index() == 0 && std::get<0>(d_) == Checkable; }
        bool comment() const { return d_.index() == 0 && std::get<0>(d_) == Comment; }
	bool named_icon() const { return std::holds_alternative<std::string>(d_); }
        std::string image_file() const { return (d_.index() == 1) ? std::get<1>(d_) : "";}
        bool has_group() const { return std::holds_alternative<RadioGroup>(d_); }
        RadioGroup group() const { return std::get<RadioGroup>(d_); }
        Gdk::RGBA color() const { return std::get<Gdk::RGBA>(d_); }
private:
        std::variant<BasicState, std::string, RadioGroup, Gdk::RGBA> d_;
};

struct LabelInfo {
    L10nText text;
    LabelDecoration decoration;
    Glib::ustring accelerator; // See the Gtk::AccelKey constructor for the format
};

struct MenuInfo;

using MenuBlockCB = std::function<std::vector<MenuInfo>(const UiContext&)>;

static const ActivateCB LegacyAction;
static const ActivateCB NoAction = [](const UiContext&){}; // NOOP callable action
static const EnabledCB LegacyEnabled;
static const EnabledCB AlwaysEnabled = [](const UiContext&){ return true; };
static const EnabledCB NeverEnabled = [](const UiContext&){ return false; };
static const CheckedCB LegacyChecked;
static const CheckedCB NotCheckable = [](const UiContext&){ return true; };

struct MenuCallbacks {
    ActivateCB handler;	/* called on mouse release */
    EnabledCB enabled = AlwaysEnabled;
    CheckedCB checked = NotCheckable;

    // Callback for custom block of menu items
    MenuBlockCB custom_block;
};

static const MenuCallbacks LegacyCallbacks = { LegacyAction, LegacyEnabled, LegacyChecked, MenuBlockCB() };
static const MenuCallbacks LegacySubMenuCallbacks = { NoAction, LegacyEnabled };
static const MenuCallbacks SubMenuCallbacks = { NoAction };

struct MenuInfo {
    LabelInfo label;
    std::vector<MenuInfo> *sub_menu;

    MenuCallbacks callbacks;

    int mid;

    bool is_separator() const { return label.text == Glib::ustring(); }
    bool is_custom_block() const { return (bool)callbacks.custom_block; }

    static MenuInfo CustomFVBlock(MenuBlockCB cb) {
        return MenuInfo{.label = {""}, .callbacks = { NoAction, LegacyEnabled, LegacyChecked, cb } };
    }
};

static const MenuInfo kMenuSeparator = {{""}};

struct MenuBarInfo {
    LabelInfo label;
    std::vector<FF::MenuInfo> *sub_menu;
    int mid;
};

Gtk::Menu* build_menu(const std::vector<FF::MenuInfo>& info, const UiContext& ui_context);

Gtk::MenuBar* build_menu_bar(const std::vector<FF::MenuBarInfo>& info, const UiContext& ui_context);

// Find the C callback set
template<typename ACT>
ACT* find_legacy_callback_set(int mid, ACT* actions) {
   int i = 0;
   while (actions[i].mid != 0) {
      if (actions[i].mid == mid) {
         return actions + i;
      }
      i++;
   }

   return NULL;
}

}
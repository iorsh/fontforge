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

    return grouper_map[g];
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

Gtk::Menu* build_menu(const std::vector<FF::MenuInfo>& info, const UiContext& ui_context) {
   Gtk::Menu* menu = new Gtk::Menu();
   Glib::RefPtr<Gtk::IconTheme> theme = Gtk::IconTheme::get_default();

   // If the menu contains custom block, we expand it before further processing
   std::vector<FF::MenuInfo> expanded_menu;
   bool has_custom_blocks = std::find_if(info.begin(), info.end(),
       [](const auto& item){ return item.is_custom_block(); }) != info.end();
   if (has_custom_blocks) {
      expanded_menu = expand_custom_blocks(info, ui_context);
   }
   const std::vector<FF::MenuInfo>& local_info = has_custom_blocks ? expanded_menu : info;

   // GTK doesn't have any signal that would be fired before the specific
   // subitem is shown. We collect enabled state checks for all subitems and
   // call them one by one from menu's show event.
   std::vector<std::function<void(void)>> enablers;
   std::vector<std::function<void(void)>> checkers;

   for (const auto& item : local_info) {
      Gtk::MenuItem* menu_item = nullptr;
      if (item.is_separator()) {
         menu_item = new Gtk::SeparatorMenuItem();
      } else if (item.label.decoration.empty()) {
         menu_item = new Gtk::MenuItem(item.label.text, true);
      } else if (item.label.decoration.has_group()) {
         RadioGroup group = item.label.decoration.group();
         Gtk::RadioButtonGroup& grouper = get_grouper(group);
         menu_item = new Gtk::RadioMenuItem(grouper, item.label.text, true);
      } else if (item.label.decoration.checkable()) {
         menu_item = new Gtk::CheckMenuItem(item.label.text, true);
      } else {
         Glib::RefPtr<Gdk::Pixbuf> pixbuf = theme->load_icon(item.label.decoration.image_file(), 16);
         Gtk::Image* img = new Gtk::Image(pixbuf);
         menu_item = new Gtk::ImageMenuItem(*img, item.label.text, true);
      }

      if (item.sub_menu) {
         Gtk::Menu* submenu = build_menu(*item.sub_menu, ui_context);
         menu_item->set_submenu(*submenu);
      }

      EnabledCB enabled_check = item.callbacks.enabled ? item.callbacks.enabled : ui_context.get_enabled_cb(item.mid);

      // Wrap the check into an action which will be called when menuitem becomes visible
      // as a part of its containing menu.
      auto enabler = [menu_item, enabled_check](){ menu_item->set_sensitive(enabled_check()); };
      enablers.push_back(enabler);

      ActivateCB action = item.callbacks.handler ? item.callbacks.handler : ui_context.get_activate_cb(item.mid);

      Gtk::CheckMenuItem* check_menu_item = dynamic_cast<Gtk::CheckMenuItem*>(menu_item);
      if (check_menu_item) {
         CheckedCB checked_cb = item.callbacks.checked ? item.callbacks.checked : ui_context.get_checked_cb(item.mid);
         // Wrap the check into an action which will be called when menuitem becomes visible
         // as a part of its containing menu.
         auto checker = [check_menu_item, checked_cb](){ check_menu_item->set_active(checked_cb()); };
         checkers.push_back(checker);

         // A check item may change its visible state either as a result of direct
         // user's action, or when initializing or due to some activity which
         // indirectly affects it. The activation signal is emitted always, but
         // we must invoke the actual callback only if the underlying state needs
         // indeed to be modified.
         ActivateCB check_action = [check_menu_item, checked_cb, action](){
            bool visible_state = check_menu_item->get_active();
            bool underlying_state = checked_cb();
            if (visible_state != underlying_state) {
                action();
            }
         };
         menu_item->signal_activate().connect(check_action);
      } else {
         menu_item->signal_activate().connect(action);
      }

      menu->append(*menu_item);
   }

   // Just call all the collected menuitem enablers
   auto on_menu_show = [enablers, checkers](){
        for (auto e : enablers) { e(); }
        for (auto c : checkers) { c(); }
   };
   menu->signal_show().connect(on_menu_show);

   return menu;
}

Gtk::MenuBar* build_menu_bar(const std::vector<FF::MenuBarInfo>& info, const UiContext& ui_context) {
   Gtk::MenuBar* menu_bar = new Gtk::MenuBar();

   for (const auto& item : info) {
      Gtk::MenuItem* menu_item = new Gtk::MenuItem(item.label.text, true);
      menu_bar->append(*menu_item);

      if (item.sub_menu) {
         Gtk::Menu* sub_menu = build_menu(*item.sub_menu, ui_context);
         menu_item->set_submenu(*sub_menu);
      }
   }

   return menu_bar;
}

}
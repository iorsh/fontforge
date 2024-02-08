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

MenuAction* find_callback_set(int mid, FVContext* fv_context) {
   MenuAction* actions = fv_context->actions;
   MenuAction* cb_set = NULL;

   // Find the C callback set
   int i = 0;
   while (actions[i].mid != 0) {
      if (actions[i].mid == mid) {
         return actions + i;
      }
      i++;
   }

   return NULL;
}

ActivateCB build_handler(int mid, FVContext* fv_context) {
   MenuAction* callback_set = find_callback_set(mid, fv_context);

   if (callback_set != NULL) {
      void (*action)(FontView*, int) = callback_set->action;
      FontView* fv = fv_context->fv;
      return [action, fv, mid](){ action(fv, mid); };
   } else {
      return NoAction;
   }
}

EnabledCB build_enabler(EnabledCB enabled, int mid, FVContext* fv_context) {
   if (enabled) {
      return enabled;
   }
   
   // No enabled callback provided, check available legacy C code callback
   MenuAction* callback_set = find_callback_set(mid, fv_context);
   if (callback_set != NULL && callback_set->is_disabled != NULL) {
      bool (*disabled_cb)(FontView*, int) = callback_set->is_disabled;
      FontView* fv = fv_context->fv;
      return [disabled_cb, fv, mid](){ return !disabled_cb(fv, mid); };
   } else {
      return AlwaysEnabled;
   }
}

Gtk::Menu* build_menu(const std::vector<FF::MenuInfo>& info, FVContext* fv_context) {
   Gtk::Menu* menu = new Gtk::Menu();
   Glib::RefPtr<Gtk::IconTheme> theme = Gtk::IconTheme::get_default();

   // GTK doesn't have any signal that would be fired before the specific
   // subitem is shown. We collect enabled state checks for all subitems and
   // call them one by one from menu's show event.
   std::vector<std::function<void(void)>> enablers;

   for (const auto& item : info) {
      Gtk::MenuItem* menu_item = nullptr;
      if (item.is_separator()) {
         menu_item = new Gtk::SeparatorMenuItem();
      } else if (item.label.decoration.empty()) {
         menu_item = new Gtk::MenuItem(item.label.text, true);
      } else {
         Glib::RefPtr<Gdk::Pixbuf> pixbuf = theme->load_icon(item.label.decoration.image_file(), 16);
         Gtk::Image* img = new Gtk::Image(pixbuf);
         menu_item = new Gtk::ImageMenuItem(*img, item.label.text, true);
      }

      if (item.sub_menu) {
         Gtk::Menu* submenu = build_menu(*item.sub_menu, fv_context);
         menu_item->set_submenu(*submenu);
      }

      ActivateCB action = item.handler ? item.handler : build_handler(item.mid, fv_context);
      menu_item->signal_activate().connect(action);

      EnabledCB enabled_check = build_enabler(item.enabled, item.mid, fv_context);

      // Wrap the check into an action which will be called when menuitem becomes visible
      // as a part of its containing menu.
      auto enabler = [menu_item, enabled_check](){ menu_item->set_sensitive(enabled_check()); };
      enablers.push_back(enabler);

      menu->append(*menu_item);
   }

   // Just call all the collected menuitem enablers
   auto on_menu_show = [enablers](){ for (auto e : enablers) { e(); } };
   menu->signal_show().connect(on_menu_show);

   return menu;
}

Gtk::MenuBar* build_menu_bar(const std::vector<FF::MenuBarInfo>& info, FVContext* fv_context) {
   Gtk::MenuBar* menu_bar = new Gtk::MenuBar();

   for (const auto& item : info) {
      Gtk::MenuItem* menu_item = new Gtk::MenuItem(item.label.text, true);
      menu_bar->append(*menu_item);

      if (item.sub_menu) {
         Gtk::Menu* sub_menu = build_menu(*item.sub_menu, fv_context);
         menu_item->set_submenu(*sub_menu);
      }
   }

   return menu_bar;
}

}
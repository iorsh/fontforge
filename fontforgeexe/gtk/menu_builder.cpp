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

namespace FF{

ActivateCB build_handler(int mid, FVContext* fv_context) {
   MenuAction* actions = fv_context->actions;
   FontView* fv = fv_context->fv;
   void (*action)(FontView*, int) = NULL;

   // Find the C handler
   int i = 0;
   while (actions[i].action != NULL) {
      if (actions[i].mid == mid) {
         action = actions[i].action;
         break;
      }
      i++;
   }

   if (action != NULL) {
      return [action, fv, mid](){ action(fv, mid); };
   } else {
      return [](){}; // NOOP callable action
   }
}

Gtk::Menu* build_menu(const std::vector<FF::MenuInfo>& info, FVContext* fv_context) {
   Gtk::Menu* menu = new Gtk::Menu();
   Glib::RefPtr<Gtk::IconTheme> theme = Gtk::IconTheme::get_default();

   for (const auto& item : info) {
      Gtk::MenuItem* menu_item = nullptr;
      if (item.is_separator()) {
         menu_item = new Gtk::SeparatorMenuItem();
      } else if (item.label.image_file.empty()) {
         menu_item = new Gtk::MenuItem(item.label.text, true);
      } else {
         Glib::RefPtr<Gdk::Pixbuf> pixbuf = theme->load_icon(item.label.image_file, 16);
         Gtk::Image* img = new Gtk::Image(pixbuf);
         menu_item = new Gtk::ImageMenuItem(*img, item.label.text, true);
      }

      ActivateCB action = item.handler ? item.handler : build_handler(item.mid, fv_context);
      menu_item->signal_activate().connect(action);

      menu->append(*menu_item);
   }

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
/* Copyright 2023 Maxim Iorsh <iorsh@users.sourceforge.net>
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with     •
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED “AS IS” AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "utils.hpp"

#include <string>
#include <vector>

Gtk::Widget* gtk_find_child(Gtk::Widget* w, const std::string& name) {
   if (w->get_name() == name) {
      return w;
   }

   Gtk::Widget* res = nullptr;
   Gtk::Container* c = dynamic_cast<Gtk::Container*>(w);

   if (c) {
      std::vector<Gtk::Widget*> children = c->get_children();
      for (size_t i = 0; res == nullptr && i < children.size(); ++i) {
         res = gtk_find_child(children[i], name);
      }
   }
   return res;
}

Gdk::ModifierType gtk_get_keyboard_state() {
   Glib::RefPtr<Gdk::Display> display =	Gdk::Display::get_default();
   GdkKeymap* keymap = display->get_keymap();
   Gdk::ModifierType state = (Gdk::ModifierType)gdk_keymap_get_modifier_state(keymap);

   return state;

   // The code below should work in GTK4.
#if 0
   Glib::RefPtr<Gdk::Seat> seat = display->get_default_seat();
   Glib::RefPtr<Gdk::Device> keyboard = seat->get_keyboard();
   return keyboard->get_modifier_state();
#endif
}

Glib::RefPtr<Gdk::Cursor> set_cursor(Gtk::Widget* widget, const Glib::ustring& name) {
    Glib::RefPtr<Gdk::Window> gdk_window = widget->get_window();
    auto old_cursor = gdk_window->get_cursor();

    Glib::RefPtr<Gdk::Cursor> new_cursor = Gdk::Cursor::create(gdk_window->get_display(), name);
    gdk_window->set_cursor(new_cursor);

    return old_cursor;
}

void unset_cursor(Gtk::Widget* widget, Glib::RefPtr<Gdk::Cursor> old_cursor) {
    if (old_cursor) {
        Glib::RefPtr<Gdk::Window> gdk_window = widget->get_window();
        gdk_window->set_cursor(old_cursor);
    }
}

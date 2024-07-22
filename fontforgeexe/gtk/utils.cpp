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
#include <iostream>

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

guint32 color_from_gdk_rgba(const Gdk::RGBA& color) {
    auto r = color.get_red_u() / 256;
    auto g = color.get_green_u() / 256;
    auto b = color.get_blue_u() / 256;
    auto a = color.get_alpha_u() / 256;
    return (r << 24) | (g << 16) | (b << 8) | a;
}

Glib::RefPtr<Gdk::Pixbuf> build_color_icon(const Gdk::RGBA& rgba, gint size) {
    Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, true, 8, size, size);

    // Fill with opaque black color to create the frame
    pixbuf->fill(0x000000ff);

    // Fill the interior with actual color
    guint32 g_color = color_from_gdk_rgba(rgba);
    Gdk::Pixbuf::create_subpixbuf(pixbuf, 1, 1, size-2, size-2)->fill(g_color);

    return pixbuf;
}

Glib::RefPtr<Gdk::Pixbuf> load_icon(const Glib::ustring &icon_name, int size) {
    Glib::RefPtr<Gtk::IconTheme> theme = Gtk::IconTheme::get_default();

    // Load icon by name from the theme
    Glib::RefPtr<Gdk::Pixbuf> pixbuf;
    if (theme->lookup_icon(icon_name, size)) {
	return theme->load_icon(icon_name, size, Gtk::ICON_LOOKUP_FORCE_SIZE);
    }

    // Use generic sad face for missing icons
    if (theme->lookup_icon("computer-fail-symbolic", size)) {
	return theme->load_icon("computer-fail-symbolic", size);
    }

    // Fallback to black square
    static const std::vector<guint8> sq(size*size, 0);
    static Glib::RefPtr<Gdk::Pixbuf> fallback_icon =
        Gdk::Pixbuf::create_from_data(sq.data(), Gdk::COLORSPACE_RGB, false, 8, size, size, 0);

    return fallback_icon;
}

static void accel_action_cb (GtkAccelGroup* accel_group, GObject* obj,
        guint keyval, GdkModifierType mods, std::function<void(void)>* action) {
    (*action)();
}

void accel_group_connect (Glib::RefPtr<Gtk::AccelGroup> accel_group, const Gtk::AccelKey& key,
			  const std::function<void(void)>& action) {
    guint keyval = key.get_key();
    GdkModifierType mod = (GdkModifierType)key.get_mod();

    gpointer user_data = new std::function<void(void)>(action);
    GClosure* closure = g_cclosure_new (G_CALLBACK (accel_action_cb), user_data, NULL);

    gtk_accel_group_connect (accel_group->gobj(), keyval, mod, GTK_ACCEL_VISIBLE, closure);
}

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
#include "font_view_shim.hpp"

#include <gtkmm-3.0/gtkmm.h>

#include "font_view.hpp"
#include "utils.hpp"

void* create_font_view(FVContext* fv_context, int width, int height) {
   return FontViewNS::create_view(fv_context, width, height);
}

void gtk_set_title(void* window, char* window_title, char* taskbar_title) {
   Gtk::Window* gtk_window = static_cast<Gtk::Window*>(window);

   if (gtk_window != nullptr) {
      gtk_window->set_title(window_title);
   }
}

GtkWidget* get_drawing_widget_c(void* window) {
   Gtk::Window* font_view_window = static_cast<Gtk::Window*>(window);
   Gtk::Widget* drawing_area = gtk_find_child(font_view_window, "CharGrid");

   return (GtkWidget*)drawing_area->gobj();
}

void fv_set_scroller_position(void* window, int32_t position) {
   Gtk::Window* font_view_window = static_cast<Gtk::Window*>(window);
   Gtk::Widget* scroller = gtk_find_child(font_view_window, "Scroller");

   dynamic_cast<Gtk::ScrolledWindow*>(scroller)->get_vadjustment()->set_value(position);
}

void fv_set_scroller_bounds(void* window,
   int32_t sb_min, int32_t sb_max, int32_t sb_pagesize) {

   Gtk::Window* font_view_window = static_cast<Gtk::Window*>(window);
   Gtk::Widget* scroller = gtk_find_child(font_view_window, "Scroller");

   Glib::RefPtr<Gtk::Adjustment> vert_adjustment = dynamic_cast<Gtk::ScrolledWindow*>(scroller)->get_vadjustment();
   vert_adjustment->set_lower(sb_min);
   vert_adjustment->set_upper(sb_max);
   vert_adjustment->set_page_size(sb_pagesize);
}

void fv_set_character_info(void* window, GString* info) {
   Gtk::Window* font_view_window = static_cast<Gtk::Window*>(window);
   Gtk::Label* character_info = static_cast<Gtk::Label*>(gtk_find_child(font_view_window, "CharInfo"));

   character_info->set_text(info->str);
}

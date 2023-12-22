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
#include "utils.hpp"

#include <gtkmm-3.0/gtkmm.h>

bool on_drawing_area_event(GdkEvent* event) {
   // Normally events automatically get to the main loop and picked from there
   // by the legacy GDraw handler. The DrawingArea::resize signal doesn't go
   // there for some reason. My best guess is that this signal is not emitted,
   // but the framework just invokes the handler directly. Here we catch it and
   // place into the main loop, so that it can reach the GDraw handler.
   if (event->type == GDK_CONFIGURE) {
      gdk_event_put(event);
      return true;
   }

   // Return false to allow further propagation of unhandled events
   return false;
}

void* create_font_view(FVContext* fv_context, int width, int height) {
   Gtk::Window* font_view_window = new Gtk::Window();
   font_view_window->set_default_size(width, height);

   Gtk::ScrolledWindow* scroller = new Gtk::ScrolledWindow();
   scroller->set_name("Scroller");
   scroller->set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_ALWAYS);
   scroller->set_overlay_scrolling(false);

   auto on_scrollbar_value_changed =
      [fv = fv_context->fv, scroll_cb = fv_context->scroll_fontview_to_position_cb, scroller]() {
         double new_position = scroller->get_vscrollbar()->get_value();
         scroll_cb(fv, new_position);
      };

   scroller->get_vscrollbar()->signal_value_changed().connect(on_scrollbar_value_changed);

   Gtk::DrawingArea* drawing_area = new Gtk::DrawingArea();
   drawing_area->set_name("CharGrid");

   // Fontforge drawing area processes events in the legacy code
   // expose, keypresses, mouse etc.
   drawing_area->signal_event().connect(&on_drawing_area_event);
   drawing_area->set_events(Gdk::ALL_EVENTS_MASK);

   scroller->add(*drawing_area);
   font_view_window->add(*scroller);

   font_view_window->show_all();

   return font_view_window;
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

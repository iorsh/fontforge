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

#include "font_view.hpp"
#include "font_view_menu.hpp"
#include "menu_builder.hpp"
#include "utils.hpp"

namespace FontViewNS {

// Create info label at the top of the Font View, which shows name and
// properties of the nost recently selected character 
Gtk::Label* make_character_info_label() {
   Gtk::Label* character_info = new Gtk::Label();
   character_info->set_name("CharInfo");
   character_info->property_margin().set_value(2);
   character_info->set_margin_left(10);
   character_info->set_hexpand(true);
   character_info->set_xalign(0); // Flush left

   // Long info string will not allow us to shrink the main window, so we
   // let it be truncated dynamically with ellipsis.
   character_info->set_ellipsize(Pango::ELLIPSIZE_END);

   // We want the info to stand out, but can't hardcode a color
   // due to the use of color themes (light, dark or even something custom)
   // We use link color to make the label sufficiently distinctive.
   Glib::RefPtr<Gtk::StyleContext> context  = character_info->get_style_context();
   Gdk::RGBA link_color = context->get_color(Gtk::STATE_FLAG_LINK);
   character_info->override_color(link_color);

   return character_info;
}


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

Gtk::Window* create_view(FVContext* fv_context, int width, int height) {
   Gtk::Window* font_view_window = new Gtk::Window();
   font_view_window->set_default_size(width, height);

   Gtk::Grid* char_grid_box = new Gtk::Grid();

   Gtk::MenuBar* top_bar = new Gtk::MenuBar();
   Gtk::MenuItem* item_file = new Gtk::MenuItem("_File", true);
   top_bar->append(*item_file);

   Gtk::Menu* file_sub_menu = new Gtk::Menu();
   item_file->set_submenu(*file_sub_menu);

   Gtk::MenuItem* item_new = new Gtk::MenuItem("_New", true);
   file_sub_menu->append(*item_new);

   Gtk::Grid* font_view_grid = new Gtk::Grid();

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
   drawing_area->set_vexpand(true);
   drawing_area->set_hexpand(true);

   // Fontforge drawing area processes events in the legacy code
   // expose, keypresses, mouse etc.
   drawing_area->signal_event().connect(&on_drawing_area_event);
   drawing_area->set_events(Gdk::ALL_EVENTS_MASK);

   Gtk::Label* character_info = make_character_info_label();

   scroller->add(*drawing_area);

   char_grid_box->attach(*character_info, 0, 0);
   char_grid_box->attach(*scroller, 0, 1);

   Gtk::HSeparator* h_sep = new Gtk::HSeparator();
   font_view_grid->attach(*top_bar, 0, 0);
   font_view_grid->attach(*h_sep, 0, 1);
   font_view_grid->attach(*char_grid_box, 0, 2);
   font_view_window->add(*font_view_grid);

   font_view_window->show_all();

   Gtk::Menu* pop_up = FF::build_menu(popup_menu, fv_context);

   auto on_my_button_press_event = [pop_up](GdkEventButton* event) {
      if (event->button == GDK_BUTTON_SECONDARY) {
          pop_up->show_all();
          pop_up->popup(event->button, event->time);
          return true;
      }
      return false;
   };

   drawing_area->signal_button_press_event().connect(on_my_button_press_event);

   return font_view_window;   
}

}
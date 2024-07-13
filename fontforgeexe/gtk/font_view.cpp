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

#include <atomic>

#include "application.hpp"
#include "font_view_shim.hpp"
#include "menu_builder.hpp"
#include "menu_ids.h"
#include "utils.hpp"

namespace FontViewNS {

// Take ownership of *p_fv_context
FontViewUiContext::FontViewUiContext(Gtk::Window* window, FVContext** p_fv_context)
    : FF::UiContext(window), legacy_context(*p_fv_context) {
    accel_group = Gtk::AccelGroup::create();
    *p_fv_context = NULL;
}

FF::ActivateCB FontViewUiContext::get_activate_cb(int mid) const {
   FVMenuAction* callback_set = FF::find_legacy_callback_set(mid, legacy_context->actions);

   if (callback_set != NULL && callback_set->action != NULL) {
      void (*action)(FontView*, int) = callback_set->action;
      FontView* fv = legacy_context->fv;
      return [action, fv, mid](const UiContext&){ action(fv, mid); };
   } else {
      return FF::NoAction;
   }
}

FF::EnabledCB FontViewUiContext::get_enabled_cb(int mid) const {
   FVMenuAction* callback_set = FF::find_legacy_callback_set(mid, legacy_context->actions);

   if (callback_set != NULL && callback_set->is_disabled != NULL) {
      bool (*disabled_cb)(FontView*, int) = callback_set->is_disabled;
      FontView* fv = legacy_context->fv;
      return [disabled_cb, fv, mid](const UiContext&){ return !disabled_cb(fv, mid); };
   } else {
      return FF::AlwaysEnabled;
   }
}

FF::CheckedCB FontViewUiContext::get_checked_cb(int mid) const {
   FVMenuAction* callback_set = FF::find_legacy_callback_set(mid, legacy_context->actions);

   if (callback_set != NULL && callback_set->is_checked != NULL) {
      bool (*checked_cb)(FontView*, int) = callback_set->is_checked;
      FontView* fv = legacy_context->fv;
      return [checked_cb, fv, mid](const UiContext&){ return checked_cb(fv, mid); };
   } else {
      return FF::NotCheckable;
   }
}

// Decide selection merge type from keyboard state
enum merge_type SelMergeType() {
    bool shift_pressed = gtk_get_keyboard_state() & Gdk::ModifierType::SHIFT_MASK;
    bool ctrl_pressed = gtk_get_keyboard_state() & Gdk::ModifierType::CONTROL_MASK;

    if (!shift_pressed && !ctrl_pressed) {
	return mt_set;
    }

    return (enum merge_type)( (shift_pressed ? mt_merge : mt_set) |
	                      (ctrl_pressed ? mt_restrict : mt_set) );
}

FF::ActivateCB FontViewUiContext::get_activate_select_cb(int mid) const {
   FVSelectMenuAction* callback_set = FF::find_legacy_callback_set(mid, legacy_context->select_actions);

   enum merge_type merge = SelMergeType();

   if (callback_set != NULL && callback_set->action != NULL) {
      void (*action)(FontView*, enum merge_type) = callback_set->action;
      FontView* fv = legacy_context->fv;
      return [action, fv, merge](const UiContext&){ action(fv, merge); };
   } else {
      return FF::NoAction;
   }
}

// Create info label at the top of the Font View, which shows name and
// properties of the most recently selected character 
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

   // Set empty label with big font to get the correct label height.
   // Due to some issue in GTK setting it now doesn't work, so we defer it until
   // after the realization.
   character_info->signal_realize().connect([character_info](){
      character_info->set_markup("<big> </big>");
   });

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

bool on_drawing_area_key(GdkEventKey* event, GdkWindow* draw_win) {
   // All keypress events belong to the top window. Some of them must go to
   // the main loop to be picked by the legacy GDraw handler. Their window
   // must be replaced, because legacy GDraw handler picks only events which
   // belong to the drawing area window.

   // The GDK reference handling is very fragile, so we *slowly* replace the
   // window...
   GdkWindow* old_event_window = event->window;
   event->window = draw_win;

   // Put a copy of the event into the main loop. The event copy handles its
   // object references by itself.
   gdk_event_put((GdkEvent*)event);

   // *Slowly* replace the window back, so that the handler caller wraps it
   // up correctly.
   event->window = old_event_window;

   // Don't handle this event any further.
   return true;
}

Gtk::Window* create_view(FVContext** p_fv_context, int width, int height) {
   Gtk::Window* font_view_window = new Gtk::Window();
   FontViewUiContext* fv_ui_context = new FontViewUiContext(font_view_window, p_fv_context);
   FVContext* fv_context = fv_ui_context->get_legacy_context();

   FF::add_top_view(*fv_ui_context);
   font_view_window->set_default_size(width, height);

   font_view_window->signal_delete_event().connect(
      [font_view_window, fv_ui_context](GdkEventAny* event){
	 auto legacy_close_cb = fv_ui_context->get_checked_cb(MID_Close);
	 bool do_close = legacy_close_cb(*fv_ui_context);
	 if (do_close) {
             FF::remove_top_view(*font_view_window);
	 }
	 // Abort or continue closing action according to do_close value
         return !do_close;
      });

   Gtk::Grid* char_grid_box = new Gtk::Grid();

   Gtk::MenuBar* top_bar = build_menu_bar(top_menu, *fv_ui_context);
   top_bar->set_name("TopBar");

   Gtk::Grid* font_view_grid = new Gtk::Grid();

   Gtk::VScrollbar* scroller = new Gtk::VScrollbar();
   scroller->set_name("Scroller");

   Gtk::DrawingArea* drawing_area = new Gtk::DrawingArea();
   drawing_area->set_name("CharGrid");
   drawing_area->set_vexpand(true);
   drawing_area->set_hexpand(true);

   // Fontforge drawing area processes events in the legacy code
   // expose, keypresses, mouse etc.
   drawing_area->signal_event().connect(&on_drawing_area_event);
   drawing_area->set_events(Gdk::ALL_EVENTS_MASK);
   drawing_area->set_can_focus(true);
   drawing_area->set_has_tooltip();

   // A hack to transfer motion event to tooltip query
   // TODO: move as member to future CharGrid class
   static std::atomic<bool> mouse_moved(false);

   auto on_query_tooltip = [fv = fv_context->fv, tooltip_cb = fv_context->tooltip_message_cb](int x, int y, bool keyboard_tooltip, const Glib::RefPtr<Gtk::Tooltip>& tooltip){
        if (mouse_moved) {
           // Mouse motion occured, dismiss the tooltip
           mouse_moved = false;
           return false;
        }

        char* tooltip_msg = tooltip_cb(fv, x, y);
        if (!tooltip_msg) {
            return false;
        }

        Glib::ustring text = Glib::Markup::escape_text(tooltip_msg);
        text = Glib::ustring::compose("<small>%1</small>", text);
        free(tooltip_msg);
        tooltip->set_markup(text);

        return true;
   };
   drawing_area->signal_query_tooltip().connect(on_query_tooltip);

   auto on_mouse_move = [drawing_area](GdkEventMotion* event){
      if (!mouse_moved) {
         mouse_moved = true;
         drawing_area->trigger_tooltip_query();
      }
      return true;
   };
   drawing_area->signal_motion_notify_event().connect(on_mouse_move);

   // Propagate scrollbar value changes to the legacy FontView code
   auto on_scrollbar_value_changed =
      [fv = fv_context->fv, scroll_cb = fv_context->scroll_fontview_to_position_cb, scroller]() {
         double new_position = scroller->get_value();
         scroll_cb(fv, new_position);
      };
   scroller->signal_value_changed().connect(on_scrollbar_value_changed);

   // Redirect mouse scrolling events from the drawing area to the scrollbar
   auto on_drawing_area_scroll = 
      [scroller](GdkEventScroll* event){ scroller->event((GdkEvent*)event); return true; };
   drawing_area->signal_scroll_event().connect(on_drawing_area_scroll);

   Gtk::Label* character_info = make_character_info_label();

   char_grid_box->attach(*character_info, 0, 0, 2, 1);
   char_grid_box->attach(*drawing_area, 0, 1);
   char_grid_box->attach(*scroller, 1, 1);

   Gtk::HSeparator* h_sep = new Gtk::HSeparator();
   font_view_grid->attach(*top_bar, 0, 0);
   font_view_grid->attach(*h_sep, 0, 1);
   font_view_grid->attach(*char_grid_box, 0, 2);
   font_view_window->add(*font_view_grid);

   font_view_window->show_all();

   // Drawing area is responsible to dispatch keypress events. Most go to the legacy code.
   GdkWindow* drawing_win = gtk_widget_get_window((GtkWidget*)(drawing_area->gobj()));
   drawing_area->signal_key_press_event().connect([drawing_win](GdkEventKey* event){
      return on_drawing_area_key(event, drawing_win);
   });

   Gtk::Menu* pop_up = FF::place_dynamic_menu(popup_menu, *fv_ui_context);

   auto on_my_button_press_event = [pop_up](GdkEventButton* event) {
      if (event->button == GDK_BUTTON_SECONDARY) {
          pop_up->show_all();
          pop_up->popup(event->button, event->time);
          return true;
      }
      return false;
   };
   drawing_area->signal_button_press_event().connect(on_my_button_press_event);

   font_view_window->add_accel_group(fv_ui_context->get_accel_group());

   return font_view_window;   
}

}
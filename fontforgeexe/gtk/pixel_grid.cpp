/* Copyright 2024 Maxim Iorsh <iorsh@users.sourceforge.net>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.

 * The name of the author may not be used to endorse or promote products
 * derived from this software without specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "pixel_grid.hpp"
#include "utils.hpp"
#include <iostream>

namespace ff::views {

bool on_drawing_area_event(GdkEvent* event);
bool on_drawing_area_key(GdkEventKey* event, Gtk::DrawingArea& drawing_area);
void on_scrollbar_value_changed(std::shared_ptr<BVContext> fv_context,
                                Gtk::Scrollbar& scroller);

PixelGrid::PixelGrid(std::shared_ptr<BVContext> context) {
    hscroller_.signal_value_changed().connect(
        [this, context]() { on_scrollbar_value_changed(context, hscroller_); });
    vscroller_.signal_value_changed().connect(
        [this, context]() { on_scrollbar_value_changed(context, vscroller_); });

    drawing_area.set_vexpand(true);
    drawing_area.set_hexpand(true);

    // Fontforge drawing area processes events in the legacy code
    // expose, keypresses, mouse etc. We reject specifically motion hints, as we
    // wish to have full mouse motion events.
    // Exposure events are also rejected, as they are processed in the legacy
    // code. Redirecting them to GTK widget would cause blanking and flickering.
    drawing_area.set_events(Gdk::ALL_EVENTS_MASK &
                            ~Gdk::POINTER_MOTION_HINT_MASK &
                            ~Gdk::EXPOSURE_MASK);
    drawing_area.signal_event().connect(&on_drawing_area_event);

    // Drawing area is responsible to dispatch keypress events. Most go to the
    // legacy code.
    drawing_area.signal_key_press_event().connect([this](GdkEventKey* event) {
        return on_drawing_area_key(event, drawing_area);
    });

    drawing_area.set_can_focus(true);

    pixel_grid_box_.attach(drawing_area, 0, 0);
    pixel_grid_box_.attach(hscroller_, 0, 1);
    pixel_grid_box_.attach(vscroller_, 1, 0);
}

Gtk::Widget& PixelGrid::get_top_widget() { return pixel_grid_box_; }

GtkWidget* PixelGrid::get_drawing_widget_c() {
    return (GtkWidget*)drawing_area.gobj();
}

void PixelGrid::set_scroller_position(bool is_vertical, int32_t position) {
    Gtk::Scrollbar& scroller = is_vertical
                                   ? static_cast<Gtk::Scrollbar&>(vscroller_)
                                   : static_cast<Gtk::Scrollbar&>(hscroller_);
    if (!scroller.has_grab()) {
        // Set the scroller only if its slider is not currently grabbed with the
        // mouse.
        scroller.get_adjustment()->set_value(position);
    }
}

void PixelGrid::set_scroller_bounds(bool is_vertical, int32_t sb_min,
                                    int32_t sb_max, int32_t sb_pagesize) {
    Gtk::Scrollbar& scroller = is_vertical
                                   ? static_cast<Gtk::Scrollbar&>(vscroller_)
                                   : static_cast<Gtk::Scrollbar&>(hscroller_);
    Glib::RefPtr<Gtk::Adjustment> adjustment = scroller.get_adjustment();
    adjustment->configure(adjustment->get_value(), sb_min, sb_max, 1,
                          sb_pagesize - 1, sb_pagesize);
}

/////////////////  EVENTS  ////////////////////

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

bool on_drawing_area_key(GdkEventKey* event, Gtk::DrawingArea& drawing_area) {
    // All keypress events belong to the top window. Some of them must go to
    // the main loop to be picked by the legacy GDraw handler. Their window
    // must be replaced, because legacy GDraw handler picks only events which
    // belong to the drawing area window.

    GdkWindow* drawing_win =
        gtk_widget_get_window((GtkWidget*)(drawing_area.gobj()));

    // The GDK reference handling is very fragile, so we *slowly* replace the
    // window...
    GdkWindow* old_event_window = event->window;
    event->window = drawing_win;

    // Put a copy of the event into the main loop. The event copy handles its
    // object references by itself.
    gdk_event_put((GdkEvent*)event);

    // *Slowly* replace the window back, so that the handler caller wraps it
    // up correctly.
    event->window = old_event_window;

    // Don't handle this event any further.
    return true;
}

void on_scrollbar_value_changed(std::shared_ptr<BVContext> bv_context,
                                Gtk::Scrollbar& scroller) {
    double new_position = scroller.get_value();
    bool is_vertical = (dynamic_cast<Gtk::VScrollbar*>(&scroller) != nullptr);

    // The horizontal scrollbar uses the opposite sign convention in the
    // legacy bitmap view code.
    if (!is_vertical) {
        new_position = -new_position;
    }

    bv_context->scroll_bitmapview_to_position_cb(bv_context->bv, is_vertical,
                                                 new_position);
}

}  // namespace ff::views

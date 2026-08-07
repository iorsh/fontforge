/* AI-generated file. Public domain. */

#include "monochrome_ribbon.hpp"

#include <algorithm>
#include <cmath>

namespace ff::widgets {

MonochromeRibbon::MonochromeRibbon(uint8_t initial_value)
    : value_(initial_value) {
    set_size_request(-1, 28);
    add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_MOTION_MASK |
               Gdk::BUTTON1_MOTION_MASK);
}

uint8_t MonochromeRibbon::value() const { return value_; }

void MonochromeRibbon::set_value(uint8_t value) {
    if (value_ == value) {
        return;
    }

    value_ = value;
    queue_draw();
    signal_value_changed_.emit(value_);
}

bool MonochromeRibbon::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
    const Gtk::Allocation alloc = get_allocation();
    const int width = alloc.get_width();
    const int height = alloc.get_height();
    if (width <= 0 || height <= 0) {
        return true;
    }

    auto gradient = Cairo::LinearGradient::create(
        0.0, 0.0, static_cast<double>(width), 0.0);
    gradient->add_color_stop_rgb(0.0, 1.0, 1.0, 1.0);
    gradient->add_color_stop_rgb(1.0, 0.0, 0.0, 0.0);
    cr->set_source(gradient);
    cr->rectangle(0.0, 0.0, static_cast<double>(width),
                  static_cast<double>(height));
    cr->fill();

    cr->set_source_rgb(0.2, 0.2, 0.2);
    cr->set_line_width(1.0);
    cr->rectangle(0.5, 0.5, static_cast<double>(width) - 1.0,
                  static_cast<double>(height) - 1.0);
    cr->stroke();

    const double marker_x = (1.0 - static_cast<double>(value_) / 255.0) *
                            static_cast<double>(width - 1);
    const double marker_tone = (value_ >= 128) ? 0.0 : 1.0;
    cr->set_source_rgb(marker_tone, marker_tone, marker_tone);
    cr->set_line_width(2.0);
    cr->move_to(marker_x, 0.0);
    cr->line_to(marker_x, static_cast<double>(height));
    cr->stroke();

    return true;
}

bool MonochromeRibbon::on_button_press_event(GdkEventButton* event) {
    if (!event || event->button != 1) {
        return false;
    }

    update_from_x(event->x);
    return true;
}

bool MonochromeRibbon::on_motion_notify_event(GdkEventMotion* event) {
    if (!event || (event->state & GDK_BUTTON1_MASK) == 0) {
        return false;
    }

    update_from_x(event->x);
    return true;
}

void MonochromeRibbon::update_from_x(double x) {
    const int width = get_allocation().get_width();
    if (width <= 1) {
        return;
    }

    const double clamped_x = std::clamp(x, 0.0, static_cast<double>(width - 1));
    const double t = clamped_x / static_cast<double>(width - 1);
    const auto new_value = static_cast<uint8_t>(std::lround((1.0 - t) * 255.0));
    set_value(new_value);
}

}  // namespace ff::widgets
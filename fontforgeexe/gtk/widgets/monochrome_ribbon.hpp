/* AI-generated file. Public domain. */
#pragma once

#include <cstdint>

#include <gtkmm.h>

namespace ff::widgets {

class MonochromeRibbon : public Gtk::DrawingArea {
 public:
    explicit MonochromeRibbon(uint8_t initial_value = 255);

    uint8_t value() const;
    void set_value(uint8_t value);

    sigc::signal<void, uint8_t>& signal_value_changed() {
        return signal_value_changed_;
    }

 protected:
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
    bool on_button_press_event(GdkEventButton* event) override;
    bool on_motion_notify_event(GdkEventMotion* event) override;

 private:
    void update_from_x(double x);

    uint8_t value_;
    sigc::signal<void, uint8_t> signal_value_changed_;
};

}  // namespace ff::widgets
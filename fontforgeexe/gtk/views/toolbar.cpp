/* Copyright 2026 Maxim Iorsh <iorsh@users.sourceforge.net>
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

#include "toolbar.hpp"

#include "../bitmap_view_shim.hpp"
#include "../utils.hpp"

namespace ff::views {

std::map<BVDevice, bvtools> ViewToolbar::tool_map_ = {
    {bvd_mouse_btn1, bvt_pencil}, {bvd_mouse_ctrl_btn1, bvt_pointer}};

BVDevice determine_device(GdkEventButton* event) {
    bool ctrl_down = (event->state & GDK_CONTROL_MASK) != 0;  // Ctrl key
    BVDevice device = bvd_undefined;

    if (event->button == GDK_BUTTON_PRIMARY) {
        device = ctrl_down ? bvd_mouse_ctrl_btn1 : bvd_mouse_btn1;
    } else if (event->button == GDK_BUTTON_MIDDLE) {
        device = ctrl_down ? bvd_mouse_ctrl_btn2 : bvd_mouse_btn2;
    } else {
        GdkInputSource src = gdk_device_get_source(event->device);
        if (src == GDK_SOURCE_PEN) {
            device = ctrl_down ? bvd_ctrl_stylus : bvd_stylus;
        } else if (src == GDK_SOURCE_ERASER) {
            device = bvd_eraser;
        }
    }

    return device;
}

ViewToolbar::ViewToolbar(const std::vector<BitmapViewTool>& bitmap_view_tools)
    : bitmap_view_tools_(bitmap_view_tools) {
    int icon_height = std::max(16, (int)(2 * ui_utils::ui_font_eX_size()));

    set_vexpand(false);
    set_valign(Gtk::ALIGN_START);

    Gtk::RadioToolButton::Group tool_group;

    for (const auto& tool : bitmap_view_tools_) {
        BVDevice device = find_device(tool.tool_id);
        Gtk::Image* icon = Gtk::make_managed<Gtk::Image>(
            make_tool_icon(tool.icon_name, device));
        Gtk::RadioToolButton* button =
            Gtk::make_managed<Gtk::RadioToolButton>(*icon);
        button->set_group(tool_group);
        button->set_tooltip_text(tool.label);
        button->set_active(device == bvd_mouse_btn1);
        tool_button_map_[tool.tool_id] = button;

        auto click_controller = create_tool_button_controller(*button);
        // gestures_ is a dummy container to keep the controller alive for the
        // lifetime of the ViewToolbar.
        gestures_.push_back(click_controller);

        // Only signal_event() agrees to catch secondary mouse clicks.
        button->signal_event().connect([this, tool](GdkEvent* event) {
            if (event->type == GDK_BUTTON_RELEASE) {
                GdkEventButton* button_event =
                    reinterpret_cast<GdkEventButton*>(event);
                on_tool_button_clicked(button_event, tool);
                return true;
            }
            return false;
        });
        add(*button);
    }
}

BVDevice ViewToolbar::find_device(bvtools tool_id) const {
    auto it = std::find_if(tool_map_.begin(), tool_map_.end(),
                           [tool_id](const std::pair<BVDevice, bvtools>& pair) {
                               return pair.second == tool_id;
                           });
    return (it == tool_map_.end()) ? bvd_undefined : it->first;
}

const BitmapViewTool& ViewToolbar::find_tool_definition(BVDevice device) const {
    auto it = tool_map_.find(device);
    if (it == tool_map_.end()) {
        return BitmapViewTool::none;
    }
    bvtools tool_id = it->second;
    return find_tool_definition(tool_id);
}

// The only purpose of this controller is to filter out Ctrl+primary click
// events, which are used to activate a secondary tool. By FontForge convention,
// the active tool button only shows the active primary tool (primary mouse
// button). Ctrl+primary click activates other tool, and this should not result
// in active UI button.
Glib::RefPtr<Gtk::GestureMultiPress> ViewToolbar::create_tool_button_controller(
    Gtk::RadioToolButton& button) const {
    auto click_controller = Gtk::GestureMultiPress::create(button);

    // Steal these definitions from gtk_button_init().
    click_controller->set_touch_only(false);
    click_controller->set_exclusive(true);
    click_controller->set_button(GDK_BUTTON_PRIMARY);

    click_controller->signal_pressed().connect(
        [this, click_controller](int n_press, double x, double y) {
            GdkEvent* current_event = gtk_get_current_event();
            if (current_event != nullptr &&
                current_event->type == GDK_BUTTON_PRESS) {
                GdkEventButton* button_event =
                    reinterpret_cast<GdkEventButton*>(current_event);
                if (button_event != nullptr) {
                    // Effectively drop Ctrl+click events.
                    if (button_event->state & GDK_CONTROL_MASK) {
                        click_controller->set_state(
                            Gtk::EventSequenceState::EVENT_SEQUENCE_CLAIMED);
                    }
                }
            }
        },
        false  // Run this handler before default ones.
    );
    click_controller->set_propagation_phase(
        Gtk::PropagationPhase::PHASE_CAPTURE);

    return click_controller;
}

Glib::RefPtr<Gdk::Pixbuf> ViewToolbar::make_tool_icon(
    const std::string& icon_name, BVDevice device) {
    // Device icons, one device can have up to two icons
    static const std::map<BVDevice, std::array<std::string, 2>> kDeviceIcons = {
        {bvd_undefined, {}},
        {bvd_mouse_btn1, {}},
        {bvd_mouse_ctrl_btn1, {"", "tool_control"}},
        {bvd_mouse_btn2, {"tool_middle_button"}},
        {bvd_mouse_ctrl_btn2, {"tool_middle_button", "tool_control"}},
        {bvd_stylus, {"tool_stylus"}},
        {bvd_ctrl_stylus, {"tool_stylus", "tool_control"}},
        {bvd_eraser, {"tool_stylus"}},
    };

    static std::map<std::pair<std::string, BVDevice>, Glib::RefPtr<Gdk::Pixbuf>>
        icon_cache;
    std::pair<std::string, BVDevice> cache_key = {icon_name, device};
    auto it = icon_cache.find(cache_key);
    if (it != icon_cache.end()) {
        return it->second;
    }

    // Create a slightly taller bitmap to accommodate the device icons in the
    // lower corners.
    int icon_width = std::max(16, (int)(2 * ui_utils::ui_font_eX_size()));
    int icon_height = icon_width;
    Glib::RefPtr<Gdk::Pixbuf> icon = Gdk::Pixbuf::create(
        Gdk::COLORSPACE_RGB, true, 8, icon_width, icon_height);
    auto raw_icon = ff::ui_utils::load_icon(icon_name, icon_width);
    raw_icon->copy_area(0, 0, raw_icon->get_width(), raw_icon->get_height(),
                        icon, 0,
                        (icon->get_height() - raw_icon->get_height()) / 2);

    const auto& dev_icons = kDeviceIcons.at(device);

    //     // Lower left corner
    //     if (!dev_icons[0].empty()) {
    //         auto dev_icon = ff::ui_utils::load_icon(dev_icons[0], icon_width
    //         / 2); int dst_x = 0; int dst_y = icon->get_height() -
    //         dev_icon->get_height(); dev_icon->composite(icon, dst_x, dst_y,
    //         dev_icon->get_width(),
    //                             dev_icon->get_height(), dst_x,
    //                             dst_y, 1.0, 1.0, Gdk::INTERP_NEAREST, 255);
    //     }

    //     // Lower right corner
    //     if (!dev_icons[1].empty()) {
    //         auto dev_icon = ff::ui_utils::load_icon(dev_icons[1], icon_width
    //         / 2); int dst_x = icon->get_width() - dev_icon->get_width(); int
    //         dst_y = icon->get_height() - dev_icon->get_height();
    //         dev_icon->composite(icon, dst_x, dst_y, dev_icon->get_width(),
    //                             dev_icon->get_height(), dst_x,
    //                             dst_y, 1.0, 1.0, Gdk::INTERP_NEAREST, 255);
    //     }

    icon_cache[cache_key] = icon;
    return icon;
}

const BitmapViewTool& ViewToolbar::find_tool_definition(bvtools tool_id) const {
    auto tool_def_it = std::find_if(
        bitmap_view_tools_.begin(), bitmap_view_tools_.end(),
        [tool_id](const BitmapViewTool& td) { return td.tool_id == tool_id; });

    return *tool_def_it;
}

void ViewToolbar::on_tool_button_clicked(GdkEventButton* event,
                                         const BitmapViewTool& tool_def) {
    BVDevice device = determine_device(event);
    if (device == bvd_undefined) return;  // unsupported device

    // Primary mouse button is already assigned to this tool, no other devices
    // can take over it.
    auto primary_tool_it = tool_map_.find(bvd_mouse_btn1);
    if (primary_tool_it != tool_map_.end() &&
        primary_tool_it->second == tool_def.tool_id) {
        return;
    }

    // Check if this tool was already assigned to another device, and if so,
    // remove it from that device.
    BVDevice existing_device = find_device(tool_def.tool_id);
    if (existing_device != bvd_undefined) {
        tool_map_.erase(existing_device);
    }

    // Find the previously active tool for this device, if any, and update its
    // button icon to the default one.
    auto it = tool_map_.find(device);
    if (it != tool_map_.end()) {
        bvtools old_tool = it->second;

        const BitmapViewTool& old_tool_def = find_tool_definition(old_tool);
        Gtk::RadioToolButton* old_button = tool_button_map_[old_tool];
        auto icon_image =
            dynamic_cast<Gtk::Image*>(old_button->get_icon_widget());
        if (icon_image) {
            auto icon_pixbuf =
                make_tool_icon(old_tool_def.icon_name, bvd_undefined);
            icon_image->set(icon_pixbuf);
        }
    }

    // Update the tool map and the button icon for the newly assigned tool.
    tool_map_[device] = tool_def.tool_id;
    auto new_button = tool_button_map_[tool_def.tool_id];
    auto icon_image = dynamic_cast<Gtk::Image*>(new_button->get_icon_widget());
    if (icon_image) {
        auto icon_pixbuf = make_tool_icon(tool_def.icon_name, device);
        icon_image->set(icon_pixbuf);
    }

    // Cursor reflects the tool assigned to primary device only.
    if (device != bvd_mouse_btn1 && device != bvd_stylus) return;

    signal_tool_clicked_.emit(tool_def);
}

}  // namespace ff::views

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

#include "bitmap_view.hpp"
#include "bitmap_view_shim.hpp"
#include "utils.hpp"

namespace ff::views {

std::map<BVDevice, bvtools> BitmapView::tool_map_ = {
    {bvd_mouse_btn1, bvt_pencil}, {bvd_mouse_ctrl_btn1, bvt_pointer}};

BitmapView::BitmapView(std::shared_ptr<BVContext> bv_context, int width,
                       int height)
    : context(bv_context), pixel_grid(bv_context) {
    // TODO(iorsh): Remove this later. The persistent width/height values are
    // currently broken, make sure they are positive so as not to break the
    // window resizing.
    width = std::max(width, 1);
    height = std::max(height, 1);

    Gtk::Grid* root_grid = Gtk::make_managed<Gtk::Grid>();

    Gtk::Box* infobar = build_infobar();
    Gtk::VBox* toolbar = build_toolbar();

    pixel_grid.get_drawing_widget().signal_motion_notify_event().connect(
        sigc::mem_fun(*this, &BitmapView::on_motion_notify_event));

    pixel_grid.get_drawing_widget().signal_button_press_event().connect(
        sigc::mem_fun(*this, &BitmapView::on_button_press_event));

    pixel_grid.get_drawing_widget().signal_key_press_event().connect(
        sigc::mem_fun(*this, &BitmapView::on_key_event), false);

    pixel_grid.get_drawing_widget().signal_key_release_event().connect(
        sigc::mem_fun(*this, &BitmapView::on_key_event), false);

    root_grid->attach(*infobar, 0, 0, 2, 1);
    root_grid->attach(*toolbar, 0, 1);
    root_grid->attach(pixel_grid.get_top_widget(), 1, 1);

    window.add(*root_grid);

    // The legacy code currently sets the cursor on its own, we must set it
    // later.
    Glib::signal_idle().connect_once([this]() {
        update_primary_cursor(find_tool_definition(tool_map_[bvd_mouse_btn1]));
    });

    window.show_all();
    window.resize(width, height);
}

Gtk::Box* BitmapView::build_infobar() {
    Gtk::Box* infobar =
        Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL);
    infobar->set_spacing(0.5 * ff::ui_utils::ui_font_em_size());
    infobar->set_margin_start(ff::ui_utils::ui_font_em_size());

    int icon_height = ff::ui_utils::ui_font_eX_size();
    auto pbuf = ff::ui_utils::load_image("right_pointer", -1, icon_height);
    Gtk::Image* right_pointer_image = Gtk::make_managed<Gtk::Image>(pbuf);
    pbuf = ff::ui_utils::load_image("press_2_ptr", -1, icon_height);
    Gtk::Image* press_2_ptr_image = Gtk::make_managed<Gtk::Image>(pbuf);

    pointer_location_.set_width_chars(12);
    pointer_location_.set_xalign(0.0);
    pointer_drag_location_.set_width_chars(12);
    pointer_drag_location_.set_xalign(0.0);

    infobar->pack_start(*right_pointer_image, Gtk::PACK_SHRINK);
    infobar->pack_start(pointer_location_, Gtk::PACK_SHRINK);
    infobar->pack_start(*press_2_ptr_image, Gtk::PACK_SHRINK);
    infobar->pack_start(pointer_drag_location_, Gtk::PACK_SHRINK);

    return infobar;
}

Gtk::VBox* BitmapView::build_toolbar() {
    int icon_height = std::max(16, (int)(2 * ui_utils::ui_font_eX_size()));

    Gtk::VBox* toolbar = Gtk::make_managed<Gtk::VBox>();
    toolbar->set_vexpand(false);
    toolbar->set_valign(Gtk::ALIGN_START);

    const std::vector<BitmapViewTool>& bitmap_view_tools =
        *static_cast<std::vector<BitmapViewTool>*>(
            context.legacy()->p_bitmap_view_tools);
    Gtk::RadioToolButton::Group tool_group;

    for (const auto& tool : bitmap_view_tools) {
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
        // lifetime of the BitmapView.
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
        toolbar->add(*button);
    }

    toolbar->add(*Gtk::make_managed<Gtk::Separator>());

    Gtk::Image* icon = Gtk::make_managed<Gtk::Image>(
        ff::ui_utils::load_icon("filerevert", icon_height));
    Gtk::ToolButton* regen_button =
        Gtk::make_managed<Gtk::ToolButton>(*icon, "Recalculate Bitmap");
    toolbar->add(*regen_button);

    return toolbar;
}

// The only purpose of this controller is to filter out Ctrl+primary click
// events, which are used to activate a secondary tool. By FontForge convention,
// the active tool button only shows the active primary tool (primary mouse
// button). Ctrl+primary click activates other tool, and this should not result
// in active UI button.
Glib::RefPtr<Gtk::GestureMultiPress> BitmapView::create_tool_button_controller(
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

static BVDevice determine_device(GdkEventButton* event) {
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

Glib::RefPtr<Gdk::Pixbuf> BitmapView::make_tool_icon(
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
    int icon_height = std::max(20, (int)(2.5 * ui_utils::ui_font_eX_size()));
    Glib::RefPtr<Gdk::Pixbuf> icon = Gdk::Pixbuf::create(
        Gdk::COLORSPACE_RGB, true, 8, icon_width, icon_height);
    auto raw_icon = ff::ui_utils::load_icon(icon_name, icon_width);
    raw_icon->copy_area(0, 0, raw_icon->get_width(), raw_icon->get_height(),
                        icon, 0,
                        (icon->get_height() - raw_icon->get_height()) / 2);

    const auto& dev_icons = kDeviceIcons.at(device);

    // Lower left corner
    if (!dev_icons[0].empty()) {
        auto dev_icon = ff::ui_utils::load_icon(dev_icons[0], icon_width / 2);
        int dst_x = 0;
        int dst_y = icon->get_height() - dev_icon->get_height();
        dev_icon->composite(icon, dst_x, dst_y, dev_icon->get_width(),
                            dev_icon->get_height(), dst_x, dst_y, 1.0, 1.0,
                            Gdk::INTERP_NEAREST, 255);
    }

    // Lower right corner
    if (!dev_icons[1].empty()) {
        auto dev_icon = ff::ui_utils::load_icon(dev_icons[1], icon_width / 2);
        int dst_x = icon->get_width() - dev_icon->get_width();
        int dst_y = icon->get_height() - dev_icon->get_height();
        dev_icon->composite(icon, dst_x, dst_y, dev_icon->get_width(),
                            dev_icon->get_height(), dst_x, dst_y, 1.0, 1.0,
                            Gdk::INTERP_NEAREST, 255);
    }

    icon_cache[cache_key] = icon;
    return icon;
}

const BitmapViewTool& BitmapView::find_tool_definition(bvtools tool_id) const {
    const std::vector<BitmapViewTool>& bitmap_view_tools =
        *static_cast<std::vector<BitmapViewTool>*>(
            context.legacy()->p_bitmap_view_tools);
    auto tool_def_it = std::find_if(
        bitmap_view_tools.begin(), bitmap_view_tools.end(),
        [tool_id](const BitmapViewTool& td) { return td.tool_id == tool_id; });

    return *tool_def_it;
}

BVDevice BitmapView::find_device(bvtools tool_id) const {
    auto it = std::find_if(tool_map_.begin(), tool_map_.end(),
                           [tool_id](const std::pair<BVDevice, bvtools>& pair) {
                               return pair.second == tool_id;
                           });
    return (it == tool_map_.end()) ? bvd_undefined : it->first;
}

void BitmapView::update_primary_cursor(const BitmapViewTool& tool_def) {
    auto display = pixel_grid.get_drawing_widget().get_display();
    auto cursor = Gdk::Cursor::create(display, tool_def.cursor_name);
    if (!cursor) {
        // Fallback to the tool icon if the cursor was not found.
        int cursor_size =
            std::max(12, (int)(1.5 * ui_utils::ui_font_eX_size()));
        auto cursor_pbuf =
            ff::ui_utils::load_icon(tool_def.icon_name, cursor_size);
        cursor = Gdk::Cursor::create(display, cursor_pbuf, cursor_size / 2,
                                     cursor_size / 2);
    }
    pixel_grid.get_drawing_widget().get_window()->set_cursor(cursor);
}

bool BitmapView::on_motion_notify_event(GdkEventMotion* event) {
    static const int kInvalidCoord = 100000;
    int pixel_x, pixel_y, tool_x = kInvalidCoord, tool_y = kInvalidCoord;

    context.legacy()->get_pixel_and_tool_coords(context.legacy()->bv, &pixel_x,
                                                &pixel_y, &tool_x, &tool_y);

    pointer_location_.set_text(std::to_string(pixel_x) +
                               ui_utils::get_list_separator() +
                               std::to_string(pixel_y));
    if (tool_x != kInvalidCoord && tool_y != kInvalidCoord)
        pointer_drag_location_.set_text(std::to_string(tool_x) +
                                        ui_utils::get_list_separator() +
                                        std::to_string(tool_y));
    else
        pointer_drag_location_.set_text("");

    // When the primary tool is the pointer, we allow the bitmap boundaries to
    // be dragged by the pointer tool. We update the cursor near the boundary
    // line to reflect the active width tool.
    guint working_state_mask = GDK_BUTTON1_MASK | GDK_BUTTON2_MASK |
                               GDK_BUTTON3_MASK | GDK_CONTROL_MASK;
    if (find_device(bvt_pointer) == bvd_mouse_btn1 &&
        !(event->state & working_state_mask)) {
        bvtools width_tool = (bvtools)context.legacy()->active_width_tool(
            context.legacy()->bv, event->x, event->y);
        BitmapViewTool cursor_def{bvt_none, "", "", ""};
        if (width_tool == bvt_setwidth) {
            cursor_def = {width_tool, "col-resize", "col-resize", ""};
        } else if (width_tool == bvt_setvwidth) {
            cursor_def = {width_tool, "row-resize", "row-resize", ""};
        } else {
            cursor_def = find_tool_definition(bvt_pointer);
        }
        update_primary_cursor(cursor_def);
    }

    return false;
}

bool BitmapView::on_key_event(GdkEventKey* event) {
    // Update the cursor when Ctrl key is pressed or released, because
    // Ctrl+primary click activates a different tool.
    if (event->keyval == GDK_KEY_Control_L ||
        event->keyval == GDK_KEY_Control_R) {
        BVDevice device = (event->type == GDK_KEY_PRESS) ? bvd_mouse_ctrl_btn1
                                                         : bvd_mouse_btn1;
        auto it = tool_map_.find(device);
        if (it != tool_map_.end()) {
            const BitmapViewTool& tool_def = find_tool_definition(it->second);
            update_primary_cursor(tool_def);
        }
    }

    // Alt key changes the cursor to Zoom-out when magnigier tool is active
    if ((event->keyval == GDK_KEY_Alt_L || event->keyval == GDK_KEY_Alt_R) &&
        find_device(bvt_magnify) == bvd_mouse_btn1) {
        BitmapViewTool cursor_def = find_tool_definition(bvt_magnify);
        if (event->type == GDK_KEY_PRESS) {
            cursor_def.icon_name = cursor_def.cursor_name = "zoom-out";
        }
        update_primary_cursor(cursor_def);
    }

    return false;
}

bool BitmapView::on_button_press_event(GdkEventButton* event) {
    BVDevice device = determine_device(event);
    if (device == bvd_undefined) return false;  // unsupported device
    auto it = tool_map_.find(device);
    if (it == tool_map_.end()) return false;  // no tool assigned to this device

    bvtools active_tool = it->second;

    // Alt key activates minifier when magnigier tool is active
    if (active_tool == bvt_magnify && (event->state & GDK_MOD1_MASK)) {
        active_tool = bvt_minify;
    }

    context.legacy()->activate_tool(context.legacy()->bv, active_tool);

    return true;
}

void BitmapView::on_tool_button_clicked(GdkEventButton* event,
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

    update_primary_cursor(tool_def);
}

}  // namespace ff::views

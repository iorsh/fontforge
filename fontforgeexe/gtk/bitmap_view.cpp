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

#include "intl.h"

namespace ff::views {

static ColorChooserMode get_color_chooser_mode(int depth) {
    switch (depth) {
        case 1:
            return ColorChooserMode::ccm_undefined;
        case 2:
            return ColorChooserMode::ccm_gray4;
        case 4:
            return ColorChooserMode::ccm_gray16;
        case 8:
            return ColorChooserMode::ccm_grayscale;
        default:
            return ColorChooserMode::ccm_undefined;
    }
}

BitmapView::BitmapView(std::shared_ptr<BVContext> bv_context, int width,
                       int height)
    : context(bv_context),
      pixel_grid(bv_context),
      toolbar_(*static_cast<std::vector<BitmapViewTool>*>(
          context.legacy()->p_bitmap_view_tools)),
      preview_(bv_context),
      color_chooser_(get_color_chooser_mode(bv_context->depth)) {
    // TODO(iorsh): Remove this later. The persistent width/height values are
    // currently broken, make sure they are positive so as not to break the
    // window resizing.
    width = std::max(width, 1);
    height = std::max(height, 1);

    Gtk::Grid* root_grid = Gtk::make_managed<Gtk::Grid>();

    Gtk::Box* infobar = build_infobar();
    build_toolbar();
    Gtk::ToolPalette* panels = build_panels();

    pixel_grid.get_drawing_widget().signal_motion_notify_event().connect(
        sigc::mem_fun(*this, &BitmapView::on_motion_notify_event));

    pixel_grid.get_drawing_widget().signal_button_press_event().connect(
        sigc::mem_fun(*this, &BitmapView::on_button_press_event));

    pixel_grid.get_drawing_widget().signal_key_press_event().connect(
        sigc::mem_fun(*this, &BitmapView::on_key_event), false);

    pixel_grid.get_drawing_widget().signal_key_release_event().connect(
        sigc::mem_fun(*this, &BitmapView::on_key_event), false);

    // For simplicity couple the preview redraw to the bitmap view drawing area
    // redraw. This is not the most efficient way to do this, but it is simple
    // and works well enough.
    pixel_grid.get_drawing_widget().signal_draw().connect(
        [this](const Cairo::RefPtr<Cairo::Context>&) {
            preview_.queue_draw();
            return false;
        });

    root_grid->attach(*infobar, 0, 0, 3, 1);
    root_grid->attach(toolbar_, 0, 1);
    root_grid->attach(pixel_grid.get_top_widget(), 1, 1);
    root_grid->attach(*panels, 2, 1);

    window.add(*root_grid);

    // The legacy code currently sets the cursor on its own, we must set it
    // later.
    Glib::signal_idle().connect_once([this]() {
        update_primary_cursor(toolbar_.find_tool_definition(bvd_mouse_btn1));
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

void BitmapView::build_toolbar() {
    toolbar_.signal_tool_clicked().connect(
        sigc::mem_fun(*this, &BitmapView::update_primary_cursor));
    toolbar_.add(*Gtk::make_managed<Gtk::Separator>());

    int icon_height = std::max(16, (int)(2 * ui_utils::ui_font_eX_size()));
    Gtk::Image* icon = Gtk::make_managed<Gtk::Image>(
        ff::ui_utils::load_icon("filerevert", icon_height));
    Gtk::ToolButton* regen_button =
        Gtk::make_managed<Gtk::ToolButton>(*icon, "Recalculate Bitmap");
    toolbar_.add(*regen_button);
}

Gtk::ToolPalette* BitmapView::build_panels() {
    Gtk::ToolPalette* panels = Gtk::make_managed<Gtk::ToolPalette>();
    panels->set_orientation(Gtk::ORIENTATION_VERTICAL);

    Gtk::ToolItemGroup* preview_panel =
        Gtk::make_managed<Gtk::ToolItemGroup>(_("Preview"));
    preview_panel->get_label_widget()->set_halign(Gtk::ALIGN_START);
    Gtk::ToolItem* preview_item = Gtk::make_managed<Gtk::ToolItem>();
    preview_item->add(preview_);
    preview_panel->insert(*preview_item, 0);
    panels->add(*preview_panel);

    Gtk::ToolItemGroup* layers_panel =
        Gtk::make_managed<Gtk::ToolItemGroup>(_("Layers"));
    layers_panel->get_label_widget()->set_halign(Gtk::ALIGN_START);
    Gtk::ToolButton* dummy2 = Gtk::make_managed<Gtk::ToolButton>("DUMMY 2");
    layers_panel->insert(*dummy2, 0);
    panels->add(*layers_panel);

    Gtk::ToolItemGroup* color_picker_panel =
        Gtk::make_managed<Gtk::ToolItemGroup>(_("Color Picker"));
    color_picker_panel->get_label_widget()->set_halign(Gtk::ALIGN_START);
    Gtk::ToolItem* color_chooser_item = Gtk::make_managed<Gtk::ToolItem>();
    color_chooser_item->add(color_chooser_);
    color_picker_panel->insert(*color_chooser_item, 0);
    panels->add(*color_picker_panel);

    return panels;
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
    if (toolbar_.find_device(bvt_pointer) == bvd_mouse_btn1 &&
        !(event->state & working_state_mask)) {
        bvtools width_tool = (bvtools)context.legacy()->active_width_tool(
            context.legacy()->bv, event->x, event->y);
        BitmapViewTool cursor_def = BitmapViewTool::none;
        if (width_tool == bvt_setwidth) {
            cursor_def = {width_tool, "col-resize", "col-resize", ""};
        } else if (width_tool == bvt_setvwidth) {
            cursor_def = {width_tool, "row-resize", "row-resize", ""};
        } else {
            cursor_def = toolbar_.find_tool_definition(bvt_pointer);
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
        const BitmapViewTool& tool_def = toolbar_.find_tool_definition(device);
        if (tool_def.tool_id != bvt_none) {
            update_primary_cursor(tool_def);
        }
    }

    // Alt key changes the cursor to Zoom-out when magnigier tool is active
    if ((event->keyval == GDK_KEY_Alt_L || event->keyval == GDK_KEY_Alt_R) &&
        toolbar_.find_device(bvt_magnify) == bvd_mouse_btn1) {
        BitmapViewTool cursor_def = toolbar_.find_tool_definition(bvt_magnify);
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

    const BitmapViewTool& tool_def = toolbar_.find_tool_definition(device);
    bvtools active_tool = tool_def.tool_id;
    if (active_tool == bvt_none)
        return false;  // no tool assigned to this device

    // Alt key activates minifier when magnigier tool is active
    if (active_tool == bvt_magnify && (event->state & GDK_MOD1_MASK)) {
        active_tool = bvt_minify;
    }

    context.legacy()->activate_tool(context.legacy()->bv, active_tool);

    return true;
}

}  // namespace ff::views

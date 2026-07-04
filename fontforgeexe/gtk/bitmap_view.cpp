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

BitmapView::BitmapView(std::shared_ptr<BVContext> bv_context, int width,
                       int height)
    : context(bv_context), pixel_grid(bv_context) {
    Gtk::Grid* root_grid = Gtk::make_managed<Gtk::Grid>();

    Gtk::Box* infobar = build_infobar();
    Gtk::VBox* toolbar = build_toolbar();

    pixel_grid.get_drawing_widget().signal_motion_notify_event().connect(
        sigc::mem_fun(*this, &BitmapView::on_motion_notify_event));

    root_grid->attach(*infobar, 0, 0, 2, 1);
    root_grid->attach(*toolbar, 0, 1);
    root_grid->attach(pixel_grid.get_top_widget(), 1, 1);

    window.add(*root_grid);

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
        Gtk::Image* icon = Gtk::make_managed<Gtk::Image>(
            ff::ui_utils::load_icon(tool.icon_name, icon_height));
        Gtk::RadioToolButton* button =
            Gtk::make_managed<Gtk::RadioToolButton>(*icon);
        button->set_group(tool_group);
        button->set_tooltip_text(tool.label);
        button->signal_clicked().connect(
            [this, tool]() { on_tool_button_clicked(tool); });
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

    return false;
}

void BitmapView::on_tool_button_clicked(const BitmapViewTool& tool_def) {
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

}  // namespace ff::views

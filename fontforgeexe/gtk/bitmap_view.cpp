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
#include "utils.hpp"

namespace ff::views {

BitmapView::BitmapView(std::shared_ptr<BVContext> context, int width,
                       int height)
    : bv_context(context), pixel_grid(context) {
    Gtk::Box* root_box = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_VERTICAL);

    Gtk::Box* infobar = build_infobar();

    pixel_grid.get_drawing_widget().signal_motion_notify_event().connect(
        sigc::mem_fun(*this, &BitmapView::on_motion_notify_event));
    root_box->pack_start(*infobar, Gtk::PACK_SHRINK);
    root_box->pack_start(pixel_grid.get_top_widget(), Gtk::PACK_EXPAND_WIDGET);

    window.add(*root_box);

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

bool BitmapView::on_motion_notify_event(GdkEventMotion* event) {
    static const int kInvalidCoord = 100000;
    int pixel_x, pixel_y, tool_x = kInvalidCoord, tool_y = kInvalidCoord;

    bv_context->get_pixel_and_tool_coords(bv_context->bv, &pixel_x, &pixel_y,
                                          &tool_x, &tool_y);

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

}  // namespace ff::views

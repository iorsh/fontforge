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

#include "bitmap_preview.hpp"

namespace ff::views {

BitmapPreview::BitmapPreview(std::shared_ptr<BVContext> context)
    : context_(std::move(context)) {
    set_vexpand(false);
    set_size_request(64, 64);
}

bool BitmapPreview::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
    const Gtk::Allocation alloc = get_allocation();
    const int width = alloc.get_width();
    const int height = alloc.get_height();

    if (width <= 0 || height <= 0) {
        return true;
    }

    cr->set_source_rgb(0.96, 0.96, 0.96);
    cr->paint();

    GImage* image = context_->gi_wrapper;
    struct _GImage* base =
        (image->list_len == 0) ? image->u.image : image->u.images[0];
    if (!base) {
        return true;
    }

    GRect src = {0, 0, base->width, base->height};
    const int src_w = src.width;
    const int src_h = src.height;
    if (src_w <= 0 || src_h <= 0) {
        return true;
    }

    int32_t x = (width - src_w) / 2;
    int32_t y = (height - src_h) / 2;
    context_->draw_gimage_in_cairo_context(cr->cobj(), image, &src, x, y);

    cr->set_source_rgb(0.35, 0.35, 0.35);
    cr->set_line_width(1.0);
    cr->rectangle(x - 0.5, y - 0.5, src_w + 1.0, src_h + 1.0);
    cr->stroke();

    return true;
}

}  // namespace ff::views

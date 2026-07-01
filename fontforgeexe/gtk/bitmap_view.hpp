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
#pragma once

#include <string>
#include <gtkmm.h>

#include "pixel_grid.hpp"
#include "c_context.h"

namespace ff::views {

class BitmapView {
 public:
    BitmapView(std::shared_ptr<BVContext> context, int width, int height);

    void set_title(const std::string& window_title,
                   const std::string& taskbar_title) {
        window.set_title(window_title);
    }

    GtkWidget* get_drawing_widget_c() {
        return pixel_grid.get_drawing_widget_c();
    }

    void set_scroller_position(bool is_vertical, int32_t position) {
        pixel_grid.set_scroller_position(is_vertical, position);
    }

    void set_scroller_bounds(bool is_vertical, int32_t sb_min, int32_t sb_max,
                             int32_t sb_pagesize) {
        pixel_grid.set_scroller_bounds(is_vertical, sb_min, sb_max,
                                       sb_pagesize);
    }

 private:
    Gtk::Box* build_infobar();

    std::shared_ptr<BVContext> bv_context;

    Gtk::Window window;
    Gtk::Label pointer_location_;
    Gtk::Label pointer_drag_location_;
    PixelGrid pixel_grid;
};

}  // namespace ff::views
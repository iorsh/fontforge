/* Copyright (C) 2026 by Maxim Iorsh <iorsh@users@sourceforge.net>
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

#include <stdint.h>

typedef struct _GtkWidget GtkWidget;
typedef struct bitmapview_context BVContext;

enum BVDevice {
    bvd_undefined = -1,
    bvd_mouse_btn1,
    bvd_mouse_ctrl_btn1,
    bvd_mouse_btn2,
    bvd_mouse_ctrl_btn2,
    bvd_stylus,
    bvd_ctrl_stylus,
    bvd_eraser
};

#ifdef __cplusplus

#include "l10n_text.hpp"

// Duplicates enum bvtools in fontforge/baseviews.h
enum bvtools {
    bvt_pointer,
    bvt_magnify,
    bvt_pencil,
    bvt_line,
    bvt_shift,
    bvt_hand,
    bvt_minify,
    bvt_max = bvt_minify,
    bvt_eyedropper,
    bvt_setwidth,
    bvt_setvwidth,
    bvt_rect,
    bvt_filledrect,
    bvt_elipse,
    bvt_filledelipse,
    bvt_max2 = bvt_filledelipse,
    bvt_none = -1,
    bvt_fliph = 0,
    bvt_flipv,
    bvt_rotate90cw,
    bvt_rotate90ccw,
    bvt_rotate180,
    bvt_skew,
    bvt_transmove
};

struct BitmapViewTool {
    bvtools tool_id;
    std::string icon_name;
    std::string cursor_name;
    L10nText label;
};

extern "C" {
#endif

// Create GTK Bitmap View window.
// Return value:
//    pointer to ff::views::BitmapView object, opaque to C code
void* create_bitmap_view(BVContext** p_bv_context, int width, int height);

// Set views::BitmapView title and taskbar title [unsupported]
void gtk_set_title(void* bv_opaque, char* window_title, char* taskbar_title);

GtkWidget* get_drawing_widget_c(void* bv_opaque);

void bv_set_scroller_position(void* bv_opaque, bool is_vertical,
                              int32_t position);

void bv_set_scroller_bounds(void* bv_opaque, bool is_vertical, int32_t sb_min,
                            int32_t sb_max, int32_t sb_pagesize);

#ifdef __cplusplus
}
#endif

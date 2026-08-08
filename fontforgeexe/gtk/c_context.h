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

#ifdef __cplusplus
extern "C" {
#endif

#include "gimage.h"

typedef struct _cairo cairo_t;
typedef struct bitmapview BitmapView;

// C structure and callback for interacting with legacy code
typedef struct bitmapview_context {
    BitmapView* bv;
    int depth;

    // Can be cast to std::vector<BitmapViewTool>*
    void* p_bitmap_view_tools;

    // Set character grid to the desired position according to the scrollbar
    void (*scroll_bitmapview_to_position_cb)(BitmapView* bv, bool is_vertical,
                                             int32_t position);

    void (*get_pixel_and_tool_coords)(BitmapView* bv, int* pixel_x,
                                      int* pixel_y, int* tool_x, int* tool_y);

    void (*activate_tool)(BitmapView* bv, int /*enum bvtools*/ tool_id);

    int /*enum bvtools*/ (*active_width_tool)(BitmapView* bv, int x, int y);

    GImage* (*create_overview_image)(BitmapView* bv, Color fg, Color bg);

    void (*draw_gimage_in_cairo_context)(cairo_t* cc, GImage* image, GRect* src,
                                         int32_t x, int32_t y);

} BVContext;

#ifdef __cplusplus
}
#endif

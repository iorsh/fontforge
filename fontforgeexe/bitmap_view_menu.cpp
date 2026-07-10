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

#include <vector>
#include <gtkmm.h>

#include "intl.h"
#include "gtk/bitmap_view_shim.hpp"

std::vector<BitmapViewTool> bitmap_view_tools = {
    {bvt_pointer, "pointer", "default", N_("Pointer")},
    {bvt_magnify, "magnify", "zoom-in", N_("Magnify (Minify with alt)")},
    {bvt_pencil, "pencil", "", N_("Set/Clear Pixels")},
    {bvt_eyedropper, "color-picker", "", N_("Pick Color")},
    {bvt_line, "line", "", N_("Draw a Line")},
    {bvt_shift, "shift", "all-scroll", N_("Shift Entire Bitmap")},
    {bvt_hand, "hand", "grab", N_("Scroll Bitmap")},
    {bvt_rect, "rectangle", "", N_("Draw Rectangle")},
    {bvt_filledrect, "rectangle_fill", "", N_("Draw Filled Rectangle")},
    {bvt_elipse, "ellipse", "", N_("Draw Ellipse")},
    {bvt_filledelipse, "ellipse_fill", "", N_("Draw Filled Ellipse")},
};
void* p_bitmap_view_tools = &bitmap_view_tools;

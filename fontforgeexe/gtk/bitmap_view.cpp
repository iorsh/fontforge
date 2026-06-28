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

namespace ff::views {

BitmapView::BitmapView(int width, int height) {
    window.set_default_size(width, height);

    // Fontforge drawing area processes events in the legacy code
    // expose, keypresses, mouse etc. We reject specifically motion hints, as we
    // wish to have full mouse motion events.
    // Exposure events are also rejected, as they are processed in the legacy
    // code. Redirecting them to GTK widget would cause blanking and flickering.
    drawing_area.set_events(Gdk::ALL_EVENTS_MASK &
                            ~Gdk::POINTER_MOTION_HINT_MASK &
                            ~Gdk::EXPOSURE_MASK);
    window.add(drawing_area);

    window.show_all();
}

GtkWidget* BitmapView::get_drawing_widget_c() {
    return (GtkWidget*)drawing_area.gobj();
}

}  // namespace ff::views
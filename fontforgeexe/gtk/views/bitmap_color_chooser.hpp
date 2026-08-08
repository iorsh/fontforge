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

#include <cstdint>

#include <gtkmm.h>

#include "../widgets/monochrome_ribbon.hpp"

namespace ff::views {

enum ColorChooserMode {
    ccm_undefined = -1,
    ccm_grayscale,
    ccm_gray16,
    ccm_gray4,
};

class BitmapColorChooser : public Gtk::VBox {
 public:
    BitmapColorChooser(ColorChooserMode mode);

    // Monochrome bitmaps are represented by an intensity value between 0 and
    // maximum palette value. E.g. a 4-bit pixel can have intensity values from
    // 0 to 15, while an 8-bit pixel can have intensity values from 0 to 255.
    // Zero value corresponds to background color, while maximum value
    // corresponds to text color.
    uint8_t intensity() const;
    void set_intensity(uint8_t intensity);

    sigc::signal<void, uint8_t>& signal_intensity_changed() {
        return signal_intensity_changed_;
    }

 private:
    void init_monochrome_ribbon();

    void init_color_chooser();
    void on_color_chooser_value_changed();

    void on_value_changed(uint8_t value);

    ColorChooserMode mode_;
    widgets::MonochromeRibbon* monochrome_ribbon_ = nullptr;
    GtkWidget* color_chooser_widget_ = nullptr;
    Gtk::Label value_label_;
    sigc::signal<void, uint8_t> signal_intensity_changed_;
};

}  // namespace ff::views

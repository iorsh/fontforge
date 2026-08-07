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

#include "bitmap_color_chooser.hpp"

#include <string>

extern "C" {
#include "ustring.h"
}

#include "../widgets/monochrome_ribbon.hpp"

namespace ff::views {

BitmapColorChooser::BitmapColorChooser(ColorChooserMode mode) : Gtk::VBox() {
    init_monochrome_ribbon();

    value_label_.set_halign(Gtk::ALIGN_START);
    add(value_label_);
}

Gdk::RGBA BitmapColorChooser::value() const {
    auto* monochrome_ribbon =
        dynamic_cast<widgets::MonochromeRibbon*>(chooser_widget_);
    Gdk::RGBA color;
    if (monochrome_ribbon) {
        color.set_grey(static_cast<float>(monochrome_ribbon->value()) / 255.0f);
    }
    return color;
}

void BitmapColorChooser::set_value(const Gdk::RGBA& value) {
    auto* monochrome_ribbon =
        dynamic_cast<widgets::MonochromeRibbon*>(chooser_widget_);
    if (monochrome_ribbon) {
        monochrome_ribbon->set_value(
            static_cast<uint8_t>(value.get_red() * 255.0f));
    }
}

void BitmapColorChooser::init_monochrome_ribbon() {
    auto* ribbon = Gtk::make_managed<widgets::MonochromeRibbon>();
    ribbon->set_size_request(256, 30);
    ribbon->signal_value_changed().connect(
        sigc::mem_fun(*this, &BitmapColorChooser::on_mono_value_changed));
    chooser_widget_ = ribbon;
    add(*chooser_widget_);

    on_mono_value_changed(ribbon->value());
}

void BitmapColorChooser::on_mono_value_changed(uint8_t value) {
    Gdk::RGBA color;
    color.set_grey(static_cast<float>(value) / 255.0f);
    char* p_label = smprintf("Color: %s", color.to_string().c_str());
    value_label_.set_text(p_label);
    free(p_label);

    signal_value_changed_.emit(color);
}

}  // namespace ff::views

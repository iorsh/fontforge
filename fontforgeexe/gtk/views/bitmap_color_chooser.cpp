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

#include <array>
#include <string>

extern "C" {
#include <gtk/gtk.h>
#include "ustring.h"
}

#include "../utils.hpp"

using ff::ui_utils::gtk_box_child_at_position;

namespace ff::views {

BitmapColorChooser::BitmapColorChooser(ColorChooserMode mode) : Gtk::VBox() {
    if (mode == ccm_grayscale) {
        init_monochrome_ribbon();
    } else if (mode == ccm_gray16 || mode == ccm_gray4) {
        init_color_chooser(mode);
    } else {
        return;
    }

    value_label_.set_halign(Gtk::ALIGN_START);
    add(value_label_);
}

Gdk::RGBA BitmapColorChooser::value() const {
    Gdk::RGBA color;
    if (monochrome_ribbon_) {
        color.set_grey(static_cast<float>(monochrome_ribbon_->value()) /
                       255.0f);
    } else if (color_chooser_widget_) {
        gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(color_chooser_widget_),
                                   color.gobj());
    }
    return color;
}

void BitmapColorChooser::set_value(const Gdk::RGBA& value) {
    if (monochrome_ribbon_) {
        monochrome_ribbon_->set_value(
            static_cast<uint8_t>(value.get_red() * 255.0f));
    } else if (color_chooser_widget_) {
        gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(color_chooser_widget_),
                                   value.gobj());
        on_color_chooser_value_changed();
    }
}

void BitmapColorChooser::init_monochrome_ribbon() {
    monochrome_ribbon_ = Gtk::make_managed<widgets::MonochromeRibbon>();
    monochrome_ribbon_->set_size_request(256, 30);
    monochrome_ribbon_->signal_value_changed().connect(
        sigc::mem_fun(*this, &BitmapColorChooser::on_mono_value_changed));
    add(*monochrome_ribbon_);

    on_mono_value_changed(monochrome_ribbon_->value());
}

void BitmapColorChooser::init_color_chooser(ColorChooserMode mode) {
    color_chooser_widget_ = gtk_color_chooser_widget_new();
    gtk_box_pack_start(GTK_BOX(gobj()), color_chooser_widget_, TRUE, TRUE, 0);
    gtk_widget_show(color_chooser_widget_);

    // WARNING: This is an egregious hack which strips the color chooser of all
    // its functionality except for the palette. Don't try this at home!
    GtkWidget* palette_box =
        gtk_box_child_at_position(GTK_BOX(color_chooser_widget_), 0);
    GtkWidget* trash1 = gtk_box_child_at_position(GTK_BOX(palette_box), 1);
    GtkWidget* trash2 = gtk_box_child_at_position(GTK_BOX(palette_box), 2);
    gtk_container_remove(GTK_CONTAINER(palette_box), trash1);
    gtk_container_remove(GTK_CONTAINER(palette_box), trash2);

    int palette_size = (mode == ccm_gray16) ? 16 : 4;
    std::vector<GdkRGBA> gray_palette(palette_size);
    for (size_t i = 0; i < gray_palette.size(); ++i) {
        const double level = i / static_cast<double>(palette_size - 1);
        gray_palette[i].red = level;
        gray_palette[i].green = level;
        gray_palette[i].blue = level;
        gray_palette[i].alpha = 1.0;
    }
    gtk_color_chooser_add_palette(GTK_COLOR_CHOOSER(color_chooser_widget_),
                                  GTK_ORIENTATION_HORIZONTAL, 4,
                                  gray_palette.size(), gray_palette.data());

    g_signal_connect_swapped(color_chooser_widget_, "notify::rgba",
                             G_CALLBACK(+[](BitmapColorChooser* self) {
                                 self->on_color_chooser_value_changed();
                             }),
                             this);

    on_color_chooser_value_changed();
}

void BitmapColorChooser::on_value_changed(const Gdk::RGBA& color) {
    char* p_label = smprintf("Color: %s", color.to_string().c_str());
    value_label_.set_text(p_label);
    free(p_label);

    signal_value_changed_.emit(color);
}

void BitmapColorChooser::on_mono_value_changed(uint8_t value) {
    Gdk::RGBA color;
    color.set_grey(static_cast<float>(value) / 255.0f);
    on_value_changed(color);
}

void BitmapColorChooser::on_color_chooser_value_changed() {
    Gdk::RGBA color = value();
    on_value_changed(color);
}

}  // namespace ff::views

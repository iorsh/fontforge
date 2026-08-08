/* Copyright 2025 Maxim Iorsh <iorsh@users.sourceforge.net>
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

#include <clocale>
#include <string>
#include <gtkmm.h>

namespace ff::ui_utils {

double ui_font_em_size();
double ui_font_eX_size();

double get_current_ppi(Gtk::Widget* w);

inline char get_decimal_point() { return *std::localeconv()->decimal_point; }

// Under some locales (e.g. French) the double values may contain commas, and in
// that case we separate the list entries by semicolons.
// See https://en.wikipedia.org/wiki/Semicolon#Data.
inline char get_list_separator() {
    return get_decimal_point() == ',' ? ';' : ',';
}

// TODO(iorsh): Integrate this function into the global log collection
void post_error(const char* title, const char* statement, ...);

Glib::RefPtr<Gdk::Cursor> set_cursor(Gtk::Widget* widget,
                                     const Glib::ustring& name);

void unset_cursor(Gtk::Widget* widget, Glib::RefPtr<Gdk::Cursor> old_cursor);

Gtk::Widget* gtk_find_child(Gtk::Widget* w, const std::string& name);
GtkWidget* gtk_box_child_at_position(GtkBox* box, int position);

Glib::RefPtr<Gdk::Pixbuf> load_image(const Glib::ustring& icon_name, int width,
                                     int height);
Glib::RefPtr<Gdk::Pixbuf> load_icon(const Glib::ustring& icon_name, int size);

guint32 color_from_gdk_rgba(const Gdk::RGBA& rgba);

Glib::RefPtr<Gdk::Pixbuf> build_color_icon(const Gdk::RGBA& color, gint size);
}  // namespace ff::ui_utils

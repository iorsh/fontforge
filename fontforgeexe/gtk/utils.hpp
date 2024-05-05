/* Copyright 2023 Maxim Iorsh <iorsh@users.sourceforge.net>
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with     •
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED “AS IS” AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#pragma once

#include <libintl.h>
#include <gtkmm-3.0/gtkmm.h>

namespace FF {

// Seamlessly localize a string using implicit constructor and conversion.
class L10nText {
   public:
    L10nText(const char* text) : text_(text) {}

    operator Glib::ustring() const {
      if (!text_.empty() && l10n_text_.empty()) {
         l10n_text_ = gettext(text_.c_str());
      }
      return l10n_text_;
    }

   private:
    Glib::ustring text_;
    mutable Glib::ustring l10n_text_;
};

}

Gtk::Widget* gtk_find_child(Gtk::Widget* w, const std::string& name);

Gdk::ModifierType gtk_get_keyboard_state();

Glib::RefPtr<Gdk::Cursor> set_cursor(Gtk::Widget* widget, const Glib::ustring& name);

void unset_cursor(Gtk::Widget* widget, Glib::RefPtr<Gdk::Cursor> old_cursor);

guint32 color_from_gdk_rgba(const Gdk::RGBA& rgba);

Glib::RefPtr<Gdk::Pixbuf> build_color_icon(const Gdk::RGBA& color, gint size);

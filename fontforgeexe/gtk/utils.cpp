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

#include "utils.hpp"

#include <glib/gprintf.h>
#include <iostream>
#include <vector>

#include "application.hpp"

namespace ff::ui_utils {

static Cairo::TextExtents ui_font_extents(const std::string& sample_text) {
    Pango::FontDescription font =
        ff::app::ColorManager::instance().style_context()->get_font();

    // Create the toy font face explicitly once and hold it in a static so its
    // reference is released cleanly at program exit, preventing the
    // FcPatternDuplicate leak that occurs inside cairo_toy_font_face_create().
    // The destructor calls unreference() to balance the implicit reference.
    struct ToyFontFace {
        Cairo::RefPtr<Cairo::ToyFontFace> face;
        explicit ToyFontFace(const std::string& family)
            : face(Cairo::ToyFontFace::create(
                  family, Cairo::FontSlant::FONT_SLANT_NORMAL,
                  Cairo::FontWeight::FONT_WEIGHT_NORMAL)) {}
        ~ToyFontFace() { face->unreference(); }
    };
    static ToyFontFace toy_face(font.get_family());

    Cairo::RefPtr<Cairo::ImageSurface> srf =
        Cairo::ImageSurface::create(Cairo::Format::FORMAT_RGB24, 100, 100);
    Cairo::RefPtr<Cairo::Context> cairo_context = Cairo::Context::create(srf);

    cairo_context->set_font_face(toy_face.face);
    cairo_context->set_font_size(font.get_size() / PANGO_SCALE *
                                 Gdk::Screen::get_default()->get_resolution() /
                                 72);

    Cairo::TextExtents extents;
    cairo_context->get_text_extents(sample_text, extents);
    return extents;
}

double ui_font_em_size() {
    Cairo::TextExtents extents = ui_font_extents("m");
    return extents.x_advance;
}

double ui_font_eX_size() {
    Cairo::TextExtents extents = ui_font_extents("X");
    return extents.height;
}

double get_current_ppi(Gtk::Widget* w) {
    Glib::RefPtr<const Gdk::Display> display = w->get_display();
    Glib::RefPtr<Gdk::Window> window = w->get_window();
    Glib::RefPtr<const Gdk::Monitor> monitor =
        display->get_monitor_at_window(window);

    Gdk::Rectangle monitor_geom;
    monitor->get_geometry(monitor_geom);
    int monitor_width_mm = monitor->get_width_mm();
    int monitor_width_px = monitor_geom.get_width();

    double ppi = 25.4 * monitor_width_px / monitor_width_mm;
    return ppi;
}

void post_error(const char* title, const char* statement, ...) {
    va_list ap;
    va_start(ap, statement);

    // Format error statement
    gchar* result_string = NULL;
    gint chars_written = g_vasprintf(&result_string, statement, ap);
    if (chars_written >= 0 && result_string != NULL) {
        // Passing use_markup=false causes GTK to embolden the text, and we
        // don't want it.
        Gtk::MessageDialog message_dlg(result_string, true, Gtk::MESSAGE_ERROR,
                                       Gtk::BUTTONS_OK, true);
        message_dlg.set_title(title);
        message_dlg.run();
        g_free(result_string);
    } else {
        std::cerr << "Error formatting statement \"" << statement << "\""
                  << std::endl;
    }

    va_end(ap);
}

Glib::RefPtr<Gdk::Cursor> set_cursor(Gtk::Widget* widget,
                                     const Glib::ustring& name) {
    if (widget == nullptr) return {};

    Glib::RefPtr<Gdk::Window> gdk_window = widget->get_window();
    if (!gdk_window) return {};

    auto old_cursor = gdk_window->get_cursor();

    Glib::RefPtr<Gdk::Cursor> new_cursor =
        Gdk::Cursor::create(gdk_window->get_display(), name);
    gdk_window->set_cursor(new_cursor);

    return old_cursor;
}

void unset_cursor(Gtk::Widget* widget, Glib::RefPtr<Gdk::Cursor> old_cursor) {
    if (widget == nullptr) return;

    Glib::RefPtr<Gdk::Window> gdk_window = widget->get_window();
    if (!gdk_window) return;

    // old_cursor is allowed to be NULL
    gdk_window->set_cursor(old_cursor);
}

Gtk::Widget* gtk_find_child(Gtk::Widget* w, const std::string& name) {
    if (w->get_name() == name) {
        return w;
    }

    Gtk::Widget* res = nullptr;
    Gtk::Container* c = dynamic_cast<Gtk::Container*>(w);

    if (c) {
        std::vector<Gtk::Widget*> children = c->get_children();
        for (size_t i = 0; res == nullptr && i < children.size(); ++i) {
            res = gtk_find_child(children[i], name);
        }
    }
    return res;
}

GtkWidget* gtk_box_child_at_position(GtkBox* box, int position) {
    if (!box) {
        return nullptr;
    }

    GList* children = gtk_container_get_children(GTK_CONTAINER(box));
    GtkWidget* found_child = nullptr;
    for (GList* link = children; link != nullptr; link = link->next) {
        GtkWidget* child_widget = GTK_WIDGET(link->data);
        int child_position = -1;
        gtk_container_child_get(GTK_CONTAINER(box), child_widget, "position",
                                &child_position, NULL);
        if (child_position == position) {
            found_child = child_widget;
            break;
        }
    }
    g_list_free(children);
    return found_child;
}

Glib::RefPtr<Gdk::Pixbuf> load_image(const Glib::ustring& icon_name, int width,
                                     int height) {
    Glib::RefPtr<Gtk::IconTheme> theme = Gtk::IconTheme::get_default();

    // Load icon by name from the theme
    auto icon_info = theme->lookup_icon(icon_name, width);
    if (icon_info) {
        Gdk::RGBA fg =
            ff::app::ColorManager::instance().get_color("theme_fg_color");
        // Gdk::RGBA fg = Gdk::RGBA("#ffaaaa");
        bool was_symbolic = icon_info.is_symbolic();
        auto pixbuf = icon_info.load_symbolic(fg, Gdk::RGBA(), Gdk::RGBA(),
                                              Gdk::RGBA(), was_symbolic);
        if (pixbuf) {
            std::cout << "Loaded symbolic icon: " << icon_name
                      << " was symbolic: " << was_symbolic << std::endl;
            return pixbuf;
        }

        if (height == 0) {
            // Assume icon is requested.
            return theme->load_icon(icon_name, width,
                                    Gtk::ICON_LOOKUP_FORCE_SIZE);
        } else {
            return Gdk::Pixbuf::create_from_file(icon_info.get_filename(),
                                                 width, height);
        }
    }

    int size = std::max(width, height);
    // Use generic sad face for missing icons
    if (theme->lookup_icon("computer-fail-symbolic", size)) {
        return theme->load_icon("computer-fail-symbolic", size,
                                Gtk::ICON_LOOKUP_FORCE_SIZE);
    }

    // Fallback to black square
    static const std::vector<guint8> sq(width * height, 0);
    static Glib::RefPtr<Gdk::Pixbuf> fallback_icon =
        Gdk::Pixbuf::create_from_data(sq.data(), Gdk::COLORSPACE_RGB, false, 8,
                                      width, height, 0);

    return fallback_icon;
}

Glib::RefPtr<Gdk::Pixbuf> load_icon(const Glib::ustring& icon_name, int size) {
    return load_image(icon_name, size, 0);
}

guint32 color_from_gdk_rgba(const Gdk::RGBA& color) {
    auto r = color.get_red_u() / 256;
    auto g = color.get_green_u() / 256;
    auto b = color.get_blue_u() / 256;
    auto a = color.get_alpha_u() / 256;
    return (r << 24) | (g << 16) | (b << 8) | a;
}

Glib::RefPtr<Gdk::Pixbuf> build_color_icon(const Gdk::RGBA& rgba, gint size) {
    Glib::RefPtr<Gdk::Pixbuf> pixbuf =
        Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, true, 8, size, size);

    // Fill with opaque black color to create the frame
    pixbuf->fill(0x000000ff);

    // Fill the interior with actual color
    guint32 g_color = color_from_gdk_rgba(rgba);
    Gdk::Pixbuf::create_subpixbuf(pixbuf, 1, 1, size - 2, size - 2)
        ->fill(g_color);

    return pixbuf;
}

}  // namespace ff::ui_utils

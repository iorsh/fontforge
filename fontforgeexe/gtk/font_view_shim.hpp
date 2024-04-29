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

#include <gtk/gtk.h>

#include "c_context.h"

#ifdef __cplusplus
extern "C" {
#endif

// Create GTK Font View window.
// Return value:
//    pointer to Gtk::Window object, opaque to C code
// Note: the created window takes ownership of p_fv_context
void* create_font_view(FVContext** p_fv_context, int width, int height);

// Set Gtk::Window title and taskbar title [unsupported]
void gtk_set_title(void* window, char* window_title, char* taskbar_title);

GtkWidget* get_drawing_widget_c(void* window);

void fv_set_scroller_position(void* window, int32_t position);

void fv_set_scroller_bounds(void* window, int32_t sb_min, int32_t sb_max, int32_t sb_pagesize);

void fv_set_character_info(void* window, GString* info);

// Resize font view window to accomodate the new drawing area size
void fv_resize_window(void* window, int width, int height);

FVMenuAction* find_callback_set(int mid, FVContext* fv_context);

void register_py_menu_item_in_gtk(const PyMenuSpec* spec, int flags);

#ifdef __cplusplus
}
#endif

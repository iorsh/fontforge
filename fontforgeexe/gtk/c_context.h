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

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// forward declare PyObject
// per http://mail.python.org/pipermail/python-dev/2003-August/037601.html
#ifndef PyObject_HEAD
typedef struct _object PyObject;
#endif

// C structures and callbacks for interacting with legacy code
typedef struct fontview FontView;
typedef struct bdffont BDFFont;
typedef struct anchorclass AnchorClass;

enum glyphlable { gl_glyph, gl_name, gl_unicode, gl_encoding };

enum merge_type { mt_set=0, mt_merge=4, mt_or=mt_merge, mt_restrict=8, mt_and=12 };

typedef struct fv_menu_action {
   int mid;
   bool (*is_disabled)(FontView *fv, int mid);	/* called before showing */
   bool (*is_checked)(FontView *fv, int mid);	/* called before showing */
   void (*action)(FontView *fv, int mid);	/* called on mouse release */
} FVMenuAction;

typedef struct fv_select_menu_action {
   int mid;
   void (*action)(FontView *fv, enum merge_type merge);	/* called on mouse release */
} FVSelectMenuAction;

#define MENUACTION_LAST { 0, NULL, NULL }

typedef struct bitmap_menu_data {
   BDFFont *bdf;
   int16_t pixelsize;
   int depth;
   bool current;
} BitmapMenuData;

typedef struct layer_menu_data {
   char* label;
   int index;
} LayerMenuData;

typedef struct anchor_menu_data {
   char* label;
   AnchorClass* ac;
} AnchorMenuData;

typedef struct encoding_menu_data {
   char* label;
   char* enc_name;
} EncodingMenuData;

enum py_menu_flag { pmf_font=1, pmf_char=2 };

struct py_menu_text {
    const char *localized;
    const char *untranslated;
    const char *identifier;
};

typedef struct py_menu_spec {
    int depth, divider;
    struct py_menu_text *levels;
    const char *shortcut_str;
    PyObject *func, *check, *data;
} PyMenuSpec;

typedef struct fontview_context {
   FontView* fv;

   // Set character grid to the desired position according to the scrollbar
   void (*scroll_fontview_to_position_cb)(FontView* fv, int32_t position);

   // Tooltip message to display for particular character
   char* (*tooltip_message_cb)(FontView* fv, int x, int y);

   // Set view to bitmap font
   void (*change_display_bitmap)(FontView *fv, BDFFont *bdf);

   // Check if the current view is set to the bitmap font
   bool (*current_display_bitmap)(FontView *fv, BDFFont *bdf);

   // Collect bitmap fonts data for menu display
   unsigned int (*collect_bitmap_data)(FontView *fv, BitmapMenuData** bitmap_data_array);

   // Set view to layer id
   void (*change_display_layer)(FontView *fv, int ly);

   // Check if the current view is set to the layer id
   bool (*current_display_layer)(FontView *fv, int ly);

   // Collect layers data for menu display
   unsigned int (*collect_layer_data)(FontView *fv, LayerMenuData** layer_data_array);

   // Open anchor pair dialog
   void (*show_anchor_pair)(FontView *fv, AnchorClass *ac);

   // Collect layers data for menu display
   unsigned int (*collect_anchor_data)(FontView *fv, AnchorMenuData** anchor_data_array);

   // Reencode to new encoding
   void (*change_encoding)(FontView *fv, const char *enc_name);

   // Force new encoding
   void (*force_encoding)(FontView *fv, const char *enc_name);

   // Check if "enc" is the current encoding
   bool (*current_encoding)(FontView *fv, const char *enc_name);

   // Collect standard and user encodings. NULL entries may exist to designate separators.
   unsigned int (*collect_encoding_data)(FontView *fv, EncodingMenuData** encoding_data_array);

   // Python callbacks for menu activation or checking if disabled
   void (*py_activate)(FontView *fv, PyObject *func, PyObject *data);
   bool (*py_check)(FontView *fv, const char *label, PyObject *check, PyObject *data);

   // Invoke external autotrace / potrace command
   void (*run_autotrace)(FontView *fv, bool ask_user_for_arguments);

   // Set glyph color (legacy format 0xaarrggbb or -10 for color chooser)
   void (*set_color)(FontView *fv, intptr_t legacy_color);

   // Menu actions per menu ID
   FVMenuAction* actions;
   FVSelectMenuAction* select_actions;
} FVContext;

#ifdef __cplusplus
}
#endif
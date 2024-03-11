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

#ifdef __cplusplus
extern "C" {
#endif

// C structures and callbacks for interacting with legacy code
typedef struct fontview FontView;
typedef struct bdffont BDFFont;
typedef struct anchorclass AnchorClass;

enum glyphlable { gl_glyph, gl_name, gl_unicode, gl_encoding };

typedef struct fv_menu_action {
   int mid;
   bool (*is_disabled)(FontView *fv, int mid);	/* called before showing */
   bool (*is_checked)(FontView *fv, int mid);	/* called before showing */
   void (*action)(FontView *fv, int mid);	/* called on mouse release */
} FVMenuAction;

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
   // Menu actions per menu ID
   FVMenuAction* actions;
} FVContext;

#ifdef __cplusplus
}
#endif

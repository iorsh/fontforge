/* Copyright 2023 Joey Sabey <github.com/Omnikron13>
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
#ifndef FONTFORGE_OPEN_DIALOG_SHIM_HPP
#define FONTFORGE_OPEN_DIALOG_SHIM_HPP

#include <gio/gio.h>

#ifdef __cplusplus
extern "C" {
#endif
const char* select_font_dialog(const char* path, const char* title);
const char* select_font_dialog_default();

void GtkFileChooserSetBookmarks(char *bookmarks);
const char* GtkFileChooserGetBookmarks(void);
void GtkFileChooserSetPrefsChangedCallback(void *data, void (*p_c)(void *));

/* Must call free_recent_files() to release the output of read_recent_files() */
char** read_recent_files();
void free_recent_files(char*** recent_files_ptr);

/* Add a new file or boost existing to the top of the list */
void add_recent_file(char* file_path);

#ifdef __cplusplus
}
#endif

#endif //FONTFORGE_OPEN_DIALOG_SHIM_HPP

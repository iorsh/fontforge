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
using namespace std;

#include <gtkmm-3.0/gtkmm.h>
using namespace Glib;
using Gio::File;

namespace FontDialog {

   // TODO: subclass this, probably?
   // Browse for a font file to open. TODO: return a file handle, or pass in a callback?
   // TODO: accept modal flag
   // TODO: add multi-file mode option..?
   RefPtr<File> open_dialog(RefPtr<File> path = {}, ustring title = {});

   void file_dialog_set_bookmarks(const std::vector<std::string>& bookmarks);
   const std::vector<std::string>& file_dialog_get_bookmarks();
   void file_dialog_set_pref_changed_callback(std::function<void(void *)>);
}
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
#include "open_dialog_shim.hpp"

#include <iostream>
#include <cstring>
#include <numeric>
using namespace std;

#include <gtkmm-3.0/gtkmm.h>
using namespace Glib;
using Gio::File;

#include "open_dialog.hpp"

// Get a selected font file as a raw C-string path, for interfacing with
// the legacy code.
const char* select_font_dialog(const char* path, const char* title) {
   auto p = path ? File::create_for_path(path) : RefPtr<File>{};
   auto t = title ?: ustring{};
   auto f = FontDialog::open_dialog(p, t)->get_path();
   auto n = f.size();
   auto s = new char[n + 1]{};
   memcpy(s, f.c_str(), n);
   return s;
}

const char* select_font_dialog_default() { return select_font_dialog(NULL, NULL); }

void GtkFileChooserSetBookmarks(char *bookmarks) {
   std::vector<std::string> b_vec;
   std::istringstream is(bookmarks);
   for (std::string bookmark; std::getline(is, bookmark, ';');) {
      // Resolve ~ as the $HOME directory
      if (bookmark.starts_with("~/")) {
         bookmark.replace(0, 1, Glib::get_home_dir());
      }
      b_vec.push_back(bookmark);
   }
   FontDialog::file_dialog_set_bookmarks(b_vec);
}

const char* GtkFileChooserGetBookmarks(void) {
   // Persistent memory buffer for C code
   static std::string bookmarks;
   std::vector<std::string> b_vec = FontDialog::file_dialog_get_bookmarks();

   if (b_vec.empty()) {
      return NULL;
   }

   // Concatenate bookmarks with semicolon
   bookmarks = std::accumulate(std::next(b_vec.begin()), b_vec.end(),
      b_vec.front(),
      [](std::string a, std::string b){ return a + ';' + b; });

   return bookmarks.c_str();
}

void GtkFileChooserSetPrefsChangedCallback(void *data, void (*p_c)(void *)) {
   FontDialog::file_dialog_set_pref_changed_callback(std::function<void(void *)>(p_c));
}

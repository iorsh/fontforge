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

char** read_recent_files() {
   /* The number of files displayed in the "File->Recent" menu */
   static const size_t MAX_RECENT = 10;

   auto recent_manager = Gtk::RecentManager::get_default();
   auto recent_items = recent_manager->get_items();

   char** recent_files = (char**)malloc(sizeof(char*) * (MAX_RECENT + 1));
   memset(recent_files, 0, sizeof(char*) * (MAX_RECENT + 1));

   for (size_t i = 0; i < MAX_RECENT && i < recent_items.size(); ++i) {
      recent_files[i] = strdup(recent_items[i]->get_uri_display().c_str());
   }

   return recent_files;
}

void free_recent_files(char*** recent_files_ptr) {
   if (recent_files_ptr == NULL) {
      return;
   } else if (*recent_files_ptr == NULL) {
      *recent_files_ptr = NULL;
      return;
   }

   char** recent_files = *recent_files_ptr;

   for (char* s = recent_files[0]; s != NULL; ++s) {
      free(s);
   }
   free(recent_files);
   *recent_files_ptr = NULL;
}

/* Add a new file or boost existing to the top of the list */
void add_recent_file(char* file_path) {
   std::string uri = Glib::filename_to_uri(file_path);
   auto recent_manager = Gtk::RecentManager::get_default();

   // Remove (only if already present) and add to force boost to the top
   if (recent_manager->has_item(uri)) {
      recent_manager->remove_item(uri);
   }
   recent_manager->add_item(uri);
}

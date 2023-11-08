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
using namespace std;

#include <gtkmm-3.0/gtkmm.h>
using namespace Glib;
using Gio::File;

import FontDialog;

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

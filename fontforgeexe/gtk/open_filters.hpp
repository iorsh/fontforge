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
#ifndef FONTFORGE_OPEN_FILTERS_HPP
#define FONTFORGE_OPEN_FILTERS_HPP

#include <string>
#include <vector>

using namespace std;

// Font formats & associated extensions copied from original UI
// TODO: optimise order/structure?
// TODO: should be derived/generated from supported formats
const vector<tuple<string, string, vector<string>>> FONT_EXTENSIONS {
   {"OUTLINE",
   "Outline Fonts", {
      "bin",
      "cef",
      "cff",
      "cid",
      "dfont",
      "gai",
      "gsf",
      "hqx",
      "ik",
      "mf",
      "otf",
      "pf3",
      "pfa",
      "pfb",
      "pt3",
      "sfd",
      "svg",
      "svgz",
      "t42",
      "ttc",
      "ttf",
      "ufo",
      "ufo",
      "woff",
      "woff2",
   }},

   {"BITMAP",
   "Bitmap Fonts", {
      "bdf",
      "bin",
      "fnt",
      "fon",
      "gf",
      "gf[0-9][0-9][0-9]",
      "gf[0-9][0-9][0-9]w",
      "hqx",
      "otb",
      "pcf",
      "pdb",
      "pk",
      "[0-9][0-9][0-9]pk",
      "pk[0-9][0-9][0-9]w",
      "pmf",
   }},

   {"TEX_BITMAP",
   "ΤεΧ Bitmap Fonts", {
      "gf",
      "gf[0-9][0-9][0-9]",
      "gf[0-9][0-9][0-9]w",
      "pk",
      "[0-9][0-9][0-9]pk",
      "pk[0-9][0-9][0-9]w",
   }},

   {"POSTSCRIPT",
   "PostScript", {
      "cef",
      "cff",
      "cid",
      "gai",
      "gsf",
      "otf",
      "pf3",
      "pfa",
      "pfb",
      "pt3",
      "t42",
   }},

   {"TRUETYPE",
   "TrueType", {
      "t42",
      "ttc",
      "ttf",
   }},

   {"OPENTYPE",
   "OpenType", {
      "otf",
      "ttf",
      "woff",
      "woff2",
   }},

   {"TYPE_1",
   "Type1", {
      "cid",
      "gsf",
      "pfa",
      "pfb",
   }},

   {"TYPE_2",
   "Type2", {
      "cef",
      "cff",
      "gai",
      "otf",
   }},

   {"TYPE_3",
   "Type3", {
      "pf3",
      "pt3",
   }},

   {"SVG",
   "SVG", {
      "svg",
      "svgz",
   }},

   {"UFO",
   "Unified Font Object", {
      "ufo",
   }},

   {"FONTFORGE_SFD",
   "FontForge's SFD", {
      "sfd",
   }},

   {"FONTFORGE_SFD_BACKUP",
   "Backup SFD", {
      "sfd~",
   }},

   {"PDF",
   "Extract from PDF", {
      "pdf",
   }},
};


// Common single-file compression extensions
// TODO: should be derived from actual capability to decompress
const vector<string> COMPRESSION_EXTENSIONS {
   "br",
   "bz",
   "bz2",
   "gz",
   "lz",
   "lz4",
   "lzma",
   "lzo",
   "sz",
   "xz",
   "z",
   "zst",
};

#endif //FONTFORGE_OPEN_FILTERS_HPP

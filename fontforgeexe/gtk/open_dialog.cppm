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
module;

using namespace std;

#include <gtkmm-3.0/gtkmm.h>
using namespace Glib;

#include "open_filters.hpp"

export module FontDialog;

export namespace FontDialog {

   // TODO: subclass this, probably?
   // Browse for a font file to open. TODO: return a file handle, or pass in a callback?
   // TODO: accept a initial/current dir
   // TODO: accept modal flag
   // TODO: add multi-file mode option..?
   RefPtr<Gio::File> open_dialog(ustring title = "Select a font") {
      auto d = Gtk::FileChooserDialog(title, Gtk::FILE_CHOOSER_ACTION_OPEN);

      d.add_button("_Open", Gtk::RESPONSE_OK);
      d.add_button("_Cancel", Gtk::RESPONSE_CANCEL);

      // Force Glyph Names
      d.add_choice(
         "force_names",
         "Force glyph names to:",
         // TODO: don't hardcode these here (or at least shift them out to a header or such?)
         {
            "",
            "agl",
            "agl_new",
            "agl_without_afii",
            "agl_with_pua",
            "greek_small_caps",
            "tex",
            "ams",
         },
         {
            "No Rename",
            "Adobe Glyph List",
            "AGL For New Fonts",
            "AGL without afii",
            "AGL with PUA",
            "Greek small caps",
            "TeX Names",
            "AMS Names",
         }
      );
      d.set_choice("force_names", "");

      // Filter combining all recognised/supported extensions/formats
      auto filter_fonts = Gtk::FileFilter::create();
      filter_fonts->set_name("All Fonts");
      d.add_filter(filter_fonts);

      // Generate file/extension filters
      // TODO: ensure this gets caches or processed at compile or somesuch
      for(auto ext : FONT_EXTENSIONS) {
         auto f = Gtk::FileFilter::create();
         f->set_name(get<1>(ext));
         for(const auto& s : get<2>(ext)) {
            auto p = "*." + s;
            f->add_pattern(p);
            filter_fonts->add_pattern(p);
            // TODO: toggle to include/exclude compressed?
            for(const auto& z : COMPRESSION_EXTENSIONS) {
               auto pz = p + "." + z;
               f->add_pattern(pz);
               filter_fonts->add_pattern(pz);
            }
         }
         d.add_filter(f);
      }

      // Fallback to open files with unrecognised extensions
      auto filter_all = Gtk::FileFilter::create();
      filter_all->set_name("All files");
      filter_all->add_pattern("*");
      d.add_filter(filter_all);

      // TODO: remember shenanigans and such?
      d.set_filter(filter_fonts);

      // Only fires on file selection - folder selection navigates down with the
      // buttons set as they are. TODO: custom behaviour to handle .sfdir format
      if(d.run() == Gtk::RESPONSE_OK) {
         // TODO: actual open logic
         return d.get_file();
      }

      // Returning dev null seems _kinda_ semantic for a cancel..? lol TODO: express this better
      return Gio::File::create_for_path("/dev/null");
   }
}
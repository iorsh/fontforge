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

#include "open_dialog.hpp"
#include "open_filters.hpp"
#include "utils.hpp"

using namespace std;
using namespace Glib;
using Gio::File;

namespace FontDialog {
   static std::vector<std::string> bookmarks;
   static std::function<void(void *)> pref_changed_cb;

   // Gtk bookmarks are managed uniformly across all Gtk applications.
   // We use FileChooser shortcuts instead, which can be managed internally.
   bool setup_bookmarks(Gtk::FileChooserDialog* dlg) {
      // Add bookmarks to the side panel
      // TODO(GTK4) Customize bookmark icon using GFileInfo
      for (const auto& b : bookmarks) {
         dlg->add_shortcut_folder(b);
      }

      // Add "Bookmark" button to the File Chooser UI.
      //
      // This is sort of a GTK3-specific hack, which relies on the internal
      // structure of the FileChooser, and thus can easily break.
      // We find the path bar and add the button next to it, so that it appears
      // where the "New Folder" button is normally found.
      Gtk::Container* c = nullptr;
      Gtk::Widget* pathbar = gtk_find_child(dlg, "GtkPathBar");
      if (pathbar) {
         Gtk::Widget* parent = pathbar->get_ancestor(GTK_TYPE_BOX);
         if (parent) {
            c = dynamic_cast<Gtk::Container*>(parent);
         }
      }

      if (!c) {
         // Failed to find a good place for "Bookmark" button
         return false;
      }

      // Create and place the "Bookmark" button with a star icon.
      Gtk::ToggleButton* bookmark_btn = new Gtk::ToggleButton("");
      bookmark_btn->set_image_from_icon_name("emblem-favorite");
      c->add(*bookmark_btn);
      bookmark_btn->show_all();

      // On folder change: check whether the current folder is bookmarked and
      // toggle "Bookmark" button accordingly.
      auto current_folder_changed_cb = [dlg, bookmark_btn](){
         std::string curr_path = dlg->get_current_folder();
         auto shortcuts = dlg->list_shortcut_folders();

         bool button_on = (std::find(shortcuts.begin(), shortcuts.end(), curr_path) != shortcuts.end());
         bookmark_btn->set_active(button_on);
      };
      dlg->signal_current_folder_changed().connect(current_folder_changed_cb);

      // On pressing "Bookmark" button: add or remove current folder to bookmarks
      auto bookmark_button_toggled_cb = [dlg, bookmark_btn](){
         bool btn_active = bookmark_btn->get_active();
         std::string curr_path = dlg->get_current_folder();
         auto shortcuts = dlg->list_shortcut_folders();
         bool path_is_shortcut = (std::find(shortcuts.begin(), shortcuts.end(), curr_path) != shortcuts.end());

         // NOTE: this signal is also activated by changing button state from within
         // current_folder_changed_cb(), so we need to check the shortcut presence
         // and not just blindly add/remove it.
         if (btn_active && !path_is_shortcut) {
            dlg->add_shortcut_folder(curr_path);
         } else if (!btn_active && path_is_shortcut) {
            dlg->remove_shortcut_folder(curr_path);
         }
      };
      bookmark_btn->signal_toggled().connect(bookmark_button_toggled_cb);

      return true;
   }

   // TODO: subclass this, probably?
   // Browse for a font file to open. TODO: return a file handle, or pass in a callback?
   // TODO: accept modal flag
   // TODO: add multi-file mode option..?
   RefPtr<File> open_dialog(RefPtr<File> path, ustring title) {
      Gtk::Main::init_gtkmm_internals();

      auto t = title != ustring{} ? title : "Open Font";

      auto d = Gtk::FileChooserDialog(t, Gtk::FILE_CHOOSER_ACTION_OPEN);

      if(path)
         d.set_current_folder(path->get_path());

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

      // Fontforge Bookmarks
      setup_bookmarks(&d);

      int response = d.run();

      // Save bookmarks if applicable
      auto new_bookmarks = d.list_shortcut_folders();
      if (bookmarks != new_bookmarks) {
         bookmarks = new_bookmarks;
         pref_changed_cb(nullptr);
      }

      // Only fires on file selection - folder selection navigates down with the
      // buttons set as they are. TODO: custom behaviour to handle .sfdir format
      if(response == Gtk::RESPONSE_OK) {
         // TODO: actual open logic
         return d.get_file();
      }

      // Returning dev null seems _kinda_ semantic for a cancel..? lol TODO: express this better
      return Gio::File::create_for_path("/dev/null");
   }

   void file_dialog_set_bookmarks(const std::vector<std::string>& bookmarks_in) {
      bookmarks = bookmarks_in;
   }

   const std::vector<std::string>& file_dialog_get_bookmarks() {
      return bookmarks;
   }

   void file_dialog_set_pref_changed_callback(std::function<void(void *)> p_c) {
      pref_changed_cb = p_c;
   }
}
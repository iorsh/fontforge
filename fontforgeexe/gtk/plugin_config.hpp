/* Copyright (C) 2026 by Maxim Iorsh <iorsh@users@sourceforge.net>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.

 * The name of the author may not be used to endorse or promote products
 * derived from this software without specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once

#include <string>
#include <vector>

#include "dialog_base.hpp"

namespace ff::dlg {

struct PluginMetadata {
    std::string name;
    std::string author;
    std::string summary;
};

class PluginConfigurationDlg final : public DialogBase {
 private:
    Gtk::ListBox plugins_;
    Gtk::ListBox suggestions_;

    PluginConfigurationDlg(GWindow parent,
                           const std::vector<PluginMetadata>& plugins_data);
    void build_plugin_list(const std::vector<PluginMetadata>& plugins_data);
    void on_plugin_summary_clicked(const std::string& name,
                                   const std::string& summary);

 public:
    // Show the dialog and return 1 on OK, 0 otherwise.
    static int show(GWindow parent,
                    const std::vector<PluginMetadata>& plugins_data);
};

}  // namespace ff::dlg

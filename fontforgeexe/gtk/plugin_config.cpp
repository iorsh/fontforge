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

#include "plugin_config.hpp"

#include "intl.h"

namespace ff::dlg {

namespace {

Gtk::Box* build_startup_mode_choice() {
    auto choice_row = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL);
    choice_row->set_hexpand(true);

    auto startup_mode_label =
        Gtk::make_managed<Gtk::Label>(_("Plugin startup mode:"));
    startup_mode_label->set_halign(Gtk::ALIGN_START);
    choice_row->pack_start(*startup_mode_label, Gtk::PACK_EXPAND_WIDGET);

    Gtk::RadioButton::Group mode_group;
    auto choice_on =
        Gtk::make_managed<Gtk::RadioButton>(mode_group, _("O_n"), true);
    choice_on->set_tooltip_text(
        _("When a new plugin is discovered it is recorded and activated"));
    choice_on->set_hexpand(true);
    choice_row->pack_start(*choice_on, Gtk::PACK_EXPAND_WIDGET);

    auto choice_off =
        Gtk::make_managed<Gtk::RadioButton>(mode_group, _("O_ff"), true);
    choice_off->set_tooltip_text(
        _("When a new plugin is discovered it is recorded but not activated"));
    choice_off->set_hexpand(true);
    choice_row->pack_start(*choice_off, Gtk::PACK_EXPAND_WIDGET);

    auto choice_ask =
        Gtk::make_managed<Gtk::RadioButton>(mode_group, _("_Ask"), true);
    choice_ask->set_tooltip_text(
        _("When a new plugin is discovered it is left unrecorded until "
          "configured in this dialog."));
    choice_ask->set_hexpand(true);
    choice_row->pack_start(*choice_ask, Gtk::PACK_EXPAND_WIDGET);

    return choice_row;
}

}  // namespace

PluginConfigurationDlg::PluginConfigurationDlg(
    GWindow parent, const std::vector<PluginMetadata>& plugins_data)
    : DialogBase(parent) {
    set_title(_("Plugin Configuration"));

    Gtk::Box* startup_mode_choice = build_startup_mode_choice();
    get_content_area()->pack_start(*startup_mode_choice, Gtk::PACK_SHRINK);

    auto label = Gtk::make_managed<Gtk::Label>(
        _("Plugins can be Loaded and Configured now.\nOther changes will take "
          "effect at next restart."));
    label->set_halign(Gtk::ALIGN_START);
    get_content_area()->pack_start(*label, Gtk::PACK_SHRINK);

    build_plugin_list(plugins_data);
    get_content_area()->pack_start(plugins_, Gtk::PACK_EXPAND_WIDGET);

    auto suggestion = Gtk::make_managed<Gtk::Label>("Dummy suggestion");
    suggestion->set_halign(Gtk::ALIGN_START);
    suggestions_.add(*suggestion);
    get_content_area()->pack_start(suggestions_, Gtk::PACK_EXPAND_WIDGET);

    show_all();
}

void PluginConfigurationDlg::build_plugin_list(
    const std::vector<PluginMetadata>& plugins_data) {
    plugins_.set_selection_mode(Gtk::SELECTION_NONE);

    for (const auto& plugin : plugins_data) {
        auto row = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL, 8);

        auto name = Gtk::make_managed<Gtk::Label>(plugin.name);
        name->set_halign(Gtk::ALIGN_START);
        name->set_hexpand(true);
        row->pack_start(*name, Gtk::PACK_EXPAND_WIDGET);

        auto state = Gtk::make_managed<Gtk::Label>("Enabled");
        state->set_halign(Gtk::ALIGN_END);
        row->pack_start(*state, Gtk::PACK_SHRINK);

        auto button = Gtk::make_managed<Gtk::Button>();
        auto icon = Gtk::make_managed<Gtk::Image>();
        icon->set_from_icon_name("elementotherinfo", Gtk::ICON_SIZE_BUTTON);
        button->set_image(*icon);
        button->set_always_show_image(true);
        button->signal_clicked().connect(sigc::bind(
            sigc::mem_fun(*this,
                          &PluginConfigurationDlg::on_plugin_summary_clicked),
            plugin.name, plugin.summary));
        row->pack_start(*button, Gtk::PACK_SHRINK);

        plugins_.add(*row);
    }
}

void PluginConfigurationDlg::on_plugin_summary_clicked(
    const std::string& name, const std::string& summary) {
    Gtk::MessageDialog dialog(*this, _("Plugin Summary"), false,
                              Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true);
    dialog.set_secondary_text(name + "\n\n" + summary);
    dialog.run();
}

int PluginConfigurationDlg::show(
    GWindow parent, const std::vector<PluginMetadata>& plugins_data) {
    PluginConfigurationDlg dialog(parent, plugins_data);
    Gtk::ResponseType result = dialog.run();
    return result == Gtk::RESPONSE_OK ? 1 : 0;
}

}  // namespace ff::dlg

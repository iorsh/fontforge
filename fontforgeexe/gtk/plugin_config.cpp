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
#include "utils.hpp"

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

Gtk::Button* build_icon_button(const std::string& icon_name,
                               sigc::slot<void> callback) {
    auto button = Gtk::make_managed<Gtk::Button>();
    int icon_height = std::max(16, (int)(2 * ui_utils::ui_font_eX_size()));
    Glib::RefPtr<Gdk::Pixbuf> pixbuf =
        ui_utils::load_icon(icon_name.c_str(), icon_height);
    auto icon = Gtk::make_managed<Gtk::Image>(pixbuf);
    button->set_image(*icon);
    button->set_always_show_image(true);
    button->signal_clicked().connect(callback);
    return button;
}

}  // namespace

PluginConfigurationDlg::PluginConfigurationDlg(
    GWindow parent, const std::vector<PluginMetadata>& plugins_data,
    const std::vector<PluginMetadata>& suggestions_data)
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
    auto plugins_frame = Gtk::make_managed<Gtk::Frame>(_("Installed plugins"));
    plugins_frame->add(plugins_);
    get_content_area()->pack_start(*plugins_frame, Gtk::PACK_EXPAND_WIDGET);

    build_suggestions_list(suggestions_data);
    auto suggestions_frame =
        Gtk::make_managed<Gtk::Frame>(_("Suggested plugins"));
    suggestions_frame->add(suggestions_);
    get_content_area()->pack_start(*suggestions_frame, Gtk::PACK_EXPAND_WIDGET);

    get_content_area()->set_spacing(ui_utils::ui_font_eX_size());
    show_all();
}

void PluginConfigurationDlg::build_plugin_list(
    const std::vector<PluginMetadata>& plugins_data) {
    plugins_.set_selection_mode(Gtk::SELECTION_NONE);

    for (const auto& plugin : plugins_data) {
        auto row = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL, 8);

        auto name = Gtk::make_managed<Gtk::Label>();
        name->set_markup("<b>" + plugin.name + "</b>\n" + plugin.summary);
        name->set_halign(Gtk::ALIGN_START);
        name->set_hexpand(true);
        row->pack_start(*name, Gtk::PACK_EXPAND_WIDGET);

        auto actions = build_action_box(plugin);
        row->pack_start(*actions, Gtk::PACK_SHRINK);

        plugins_.add(*row);
    }
}

void PluginConfigurationDlg::build_suggestions_list(
    const std::vector<PluginMetadata>& suggestions_data) {
    suggestions_.set_selection_mode(Gtk::SELECTION_NONE);

    for (const auto& plugin : suggestions_data) {
        auto row = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL, 8);

        // Create empty icon of the same size as in build_plugin_list() to
        // maintain alignment.
        int icon_height = std::max(16, (int)(2 * ui_utils::ui_font_eX_size()));
        Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create(
            Gdk::COLORSPACE_RGB, true, 8, icon_height, icon_height);
        pixbuf->fill(0x00000000);  // Transparent
        auto icon = Gtk::make_managed<Gtk::Image>(pixbuf);
        row->pack_start(*icon, Gtk::PACK_SHRINK);

        auto name = Gtk::make_managed<Gtk::Label>();
        std::string details = "<b>" + plugin.name + "</b>\n" + plugin.summary;
        name->set_markup(details);
        name->set_halign(Gtk::ALIGN_START);
        name->set_hexpand(true);
        row->pack_start(*name, Gtk::PACK_EXPAND_WIDGET);

        if (!plugin.url.empty()) {
            auto show_uri_cb = [this, plugin]() {
                gtk_show_uri_on_window(GTK_WINDOW(this->gobj()),
                                       plugin.url.c_str(), GDK_CURRENT_TIME,
                                       nullptr);
            };
            auto url_button =
                build_icon_button("applications-internet", show_uri_cb);
            url_button->set_vexpand(false);
            url_button->set_valign(Gtk::ALIGN_CENTER);
            row->pack_start(*url_button, Gtk::PACK_SHRINK);
        }

        suggestions_.add(*row);
        suggestions_.add(
            *Gtk::make_managed<Gtk::Separator>(Gtk::ORIENTATION_HORIZONTAL));
    }
}

Gtk::Box* PluginConfigurationDlg::build_action_box(
    const PluginMetadata& plugin) {
    auto actions = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL, 4);
    actions->set_vexpand(false);
    actions->set_valign(Gtk::ALIGN_CENTER);

    auto enabled_switch = Gtk::make_managed<Gtk::Switch>();
    enabled_switch->set_active(plugin.enabled);
    enabled_switch->set_vexpand(false);
    enabled_switch->set_halign(Gtk::ALIGN_START);
    enabled_switch->set_valign(Gtk::ALIGN_CENTER);
    actions->pack_start(*enabled_switch, Gtk::PACK_SHRINK);

    auto show_summary_cb = sigc::bind(
        sigc::mem_fun(*this,
                      &PluginConfigurationDlg::on_plugin_summary_clicked),
        plugin);
    auto info_button = build_icon_button("elementotherinfo", show_summary_cb);
    actions->pack_start(*info_button, Gtk::PACK_SHRINK);

    auto config_button = build_icon_button("fileprefs", show_summary_cb);
    actions->pack_start(*config_button, Gtk::PACK_SHRINK);

    return actions;
}

void PluginConfigurationDlg::on_plugin_summary_clicked(
    const PluginMetadata& plugin) {
    Gtk::MessageDialog dialog(*this, _("Plugin Summary"), false,
                              Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true);
    dialog.set_secondary_text(plugin.name + "\n\n" + plugin.summary);
    dialog.run();
}

int PluginConfigurationDlg::show(
    GWindow parent, const std::vector<PluginMetadata>& plugins_data,
    const std::vector<PluginMetadata>& suggestions_data) {
    PluginConfigurationDlg dialog(parent, plugins_data, suggestions_data);
    Gtk::ResponseType result = dialog.run();
    return result == Gtk::RESPONSE_OK ? 1 : 0;
}

}  // namespace ff::dlg

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

#include "show_histogram.hpp"

#include "application.hpp"
#include "utils.hpp"
#include "intl.h"

extern "C" {
#include "ustring.h"
}

namespace ff::dlg {

namespace {
int s_bar_width = 6;
int s_moving_average = 1;
}  // namespace

ShowHistogramDlg::ShowHistogramDlg(GWindow parent, const HistogramData& data)
    : DialogBase(parent), data_(data) {
    set_title(data.title);
    set_help_context("ui/dialogs/histogram.html");

    auto histogram = Gtk::make_managed<ff::widgets::Histogram>();
    std::vector<int> bar_values;
    for (const auto& bar : data.bars) {
        bar_values.push_back(static_cast<int>(bar.value));
    }
    histogram->set_values(bar_values);
    histogram->set_lower_bound(data.lower_bound);
    histogram->set_tooltip_text_callback(
        [this](size_t index) { return get_tooltip_text(index); });
    histogram->set_bar_width(s_bar_width);
    histogram->set_moving_average_window(s_moving_average);

    auto histogram_scroll = Gtk::make_managed<Gtk::ScrolledWindow>();
    histogram_scroll->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_NEVER);
    histogram_scroll->set_overlay_scrolling(false);
    histogram_scroll->add(*histogram);
    get_content_area()->pack_start(*histogram_scroll, Gtk::PACK_EXPAND_WIDGET);
    get_content_area()->pack_start(*build_control_box(histogram),
                                   Gtk::PACK_SHRINK);

    auto primary_box = Gtk::make_managed<Gtk::Box>(
        Gtk::ORIENTATION_HORIZONTAL, 0.5 * ff::ui_utils::ui_font_em_size());
    auto primary_label = Gtk::make_managed<Gtk::Label>(data.primary_label);
    auto label_group = Gtk::SizeGroup::create(Gtk::SIZE_GROUP_HORIZONTAL);
    primary_label->set_xalign(0.0);
    primary_label->set_valign(Gtk::ALIGN_CENTER);
    label_group->add_widget(*primary_label);
    primary_box->pack_start(*primary_label, Gtk::PACK_SHRINK);

    auto primary_entry = Gtk::make_managed<Gtk::Entry>();
    primary_entry->set_hexpand(true);
    primary_entry->set_activates_default();
    primary_box->pack_start(*primary_entry, Gtk::PACK_EXPAND_WIDGET);
    get_content_area()->pack_start(*primary_box, Gtk::PACK_SHRINK);

    auto secondary_box = Gtk::make_managed<Gtk::Box>(
        Gtk::ORIENTATION_HORIZONTAL, 0.5 * ff::ui_utils::ui_font_em_size());
    auto secondary_label = Gtk::make_managed<Gtk::Label>(data.secondary_label);
    secondary_label->set_xalign(0.0);
    secondary_label->set_valign(Gtk::ALIGN_CENTER);
    label_group->add_widget(*secondary_label);
    secondary_box->pack_start(*secondary_label, Gtk::PACK_SHRINK);

    auto secondary_entry = Gtk::make_managed<Gtk::Entry>();
    secondary_entry->set_hexpand(true);
    secondary_entry->set_activates_default();
    secondary_box->pack_start(*secondary_entry, Gtk::PACK_EXPAND_WIDGET);
    get_content_area()->pack_start(*secondary_box, Gtk::PACK_SHRINK);

    show_all();
}

Gtk::Box* ShowHistogramDlg::build_control_box(
    ff::widgets::Histogram* histogram) {
    auto controls_box = Gtk::make_managed<Gtk::Box>(
        Gtk::ORIENTATION_HORIZONTAL, 0.5 * ff::ui_utils::ui_font_em_size());

    auto average_label = Gtk::make_managed<Gtk::Label>(_("Smoothing window:"));
    average_label->set_halign(Gtk::ALIGN_START);
    average_label->set_valign(Gtk::ALIGN_CENTER);
    controls_box->pack_start(*average_label, Gtk::PACK_SHRINK);

    auto average_entry = Gtk::make_managed<Gtk::SpinButton>(
        Gtk::Adjustment::create(s_moving_average, 1, 99, 2, 10, 0), 1, 0);
    average_entry->set_numeric(true);
    average_entry->set_snap_to_ticks(true);
    average_entry->set_width_chars(4);
    average_entry->set_valign(Gtk::ALIGN_CENTER);
    average_entry->set_activates_default();
    average_entry->signal_value_changed().connect([average_entry, histogram]() {
        s_moving_average = average_entry->get_value_as_int();
        histogram->set_moving_average_window(s_moving_average);
    });
    controls_box->pack_start(*average_entry, Gtk::PACK_SHRINK);

    auto bar_width_label = Gtk::make_managed<Gtk::Label>(_("Bar width:"));
    bar_width_label->set_halign(Gtk::ALIGN_START);
    bar_width_label->set_valign(Gtk::ALIGN_CENTER);
    controls_box->pack_start(*bar_width_label, Gtk::PACK_SHRINK);

    auto bar_width_entry = Gtk::make_managed<Gtk::SpinButton>(
        Gtk::Adjustment::create(s_bar_width, 1, 100, 1, 5, 0), 1, 0);
    bar_width_entry->set_numeric(true);
    bar_width_entry->set_width_chars(4);
    bar_width_entry->set_valign(Gtk::ALIGN_CENTER);
    bar_width_entry->set_activates_default();
    bar_width_entry->signal_value_changed().connect(
        [bar_width_entry, histogram]() {
            s_bar_width = bar_width_entry->get_value_as_int();
            histogram->set_bar_width(s_bar_width);
        });
    controls_box->pack_start(*bar_width_entry, Gtk::PACK_SHRINK);

    return controls_box;
}

std::string ShowHistogramDlg::get_tooltip_text(size_t bar_index) const {
    if (bar_index >= data_.bars.size()) {
        return "";
    }

    const int label = static_cast<int>(bar_index) + data_.lower_bound;
    const auto& bar = data_.bars[bar_index];

    char* p_width_label = smprintf(_("Width: %d"), label);
    char* p_count_label = smprintf(_("Count: %u"), bar.value);
    std::string tooltip_text(p_width_label);
    tooltip_text += "\n" + std::string(p_count_label);
    free(p_width_label);
    free(p_count_label);

    if (!bar.glyph_names.empty()) {
        tooltip_text += "\n";
    }

    for (const auto& name : bar.glyph_names) {
        if (tooltip_text.back() != '\n') {
            tooltip_text += " ";
        }
        tooltip_text += name;
    }

    return tooltip_text;
}

bool ShowHistogramDlg::show(GWindow parent, const HistogramData& data) {
    ShowHistogramDlg dialog(parent, data);
    return dialog.run() == Gtk::RESPONSE_OK;
}

void show_histogram_dialog(GWindow parent, const HistogramData& data) {
    ff::app::GtkApp();
    ShowHistogramDlg::show(parent, data);
}

}  // namespace ff::dlg

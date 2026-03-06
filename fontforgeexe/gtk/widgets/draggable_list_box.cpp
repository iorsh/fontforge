/* Copyright 2026 Maxim Iorsh <iorsh@users.sourceforge.net>
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

#include "draggable_list_box.hpp"

#include <iostream>
#include "../utils.hpp"

namespace ff::widgets {

namespace {

constexpr const char* kDragTarget = "FF_LISTBOX_DRAGGABLE_ROW";

bool is_separator_row(Gtk::ListBoxRow* candidate) {
    return candidate && dynamic_cast<Gtk::Separator*>(candidate->get_child());
}

Gtk::ListBoxRow* get_separator_drop_row(Gtk::ListBox& list, int y) {
    auto* row = list.get_row_at_y(y);
    if (is_separator_row(row)) {
        return row;
    }
    if (!row) {
        return nullptr;
    }

    int row_index = row->get_index();
    auto* prev_row = list.get_row_at_index(row_index - 1);
    auto* next_row = list.get_row_at_index(row_index + 1);

    auto allocation = row->get_allocation();
    int row_midpoint = allocation.get_y() + allocation.get_height() / 2;

    auto* preferred = (y < row_midpoint) ? prev_row : next_row;
    auto* fallback = (preferred == prev_row) ? next_row : prev_row;

    if (is_separator_row(preferred)) {
        return preferred;
    }
    if (is_separator_row(fallback)) {
        return fallback;
    }
    return nullptr;
}

Gtk::EventBox* build_drag_handle() {
    int icon_height = std::max(16, (int)(2 * ui_utils::ui_font_eX_size()));
    Glib::RefPtr<Gdk::Pixbuf> pixbuf =
        ui_utils::load_icon("view-more-symbolic", icon_height);
    auto icon = Gtk::make_managed<Gtk::Image>(pixbuf);
    auto handle = Gtk::make_managed<Gtk::EventBox>();
    handle->add(*icon);
    handle->set_visible_window(false);
    return handle;
}

}  // namespace

DraggableListBox::DraggableListBox() {
    std::vector<Gtk::TargetEntry> targets = {
        Gtk::TargetEntry(kDragTarget, Gtk::TARGET_SAME_APP)};

    drag_dest_set(targets, Gtk::DEST_DEFAULT_MOTION | Gtk::DEST_DEFAULT_DROP,
                  Gdk::ACTION_MOVE);

    signal_drag_motion().connect(
        sigc::mem_fun(*this, &DraggableListBox::on_drag_motion));
    signal_drag_leave().connect(
        sigc::mem_fun(*this, &DraggableListBox::on_drag_leave));
    signal_drag_drop().connect(
        sigc::mem_fun(*this, &DraggableListBox::on_drag_drop));
    signal_drag_data_received().connect(
        sigc::mem_fun(*this, &DraggableListBox::on_drag_data_received));
}

void DraggableListBox::add(Gtk::Widget& child) {
    auto* row_widget = dynamic_cast<Gtk::Box*>(&child);
    if (!row_widget) {
        std::cerr
            << "DraggableListBox only supports adding Gtk::Box widgets as rows."
            << std::endl;
        return;
    }

    // Separators are also highlighted to indicate drop position, so we need one
    // before the first item as well.
    if (get_children().empty()) {
        Gtk::ListBox::add(
            *Gtk::make_managed<Gtk::Separator>(Gtk::ORIENTATION_HORIZONTAL));
    }

    auto handle = build_drag_handle();

    // Put handle at the start of the row
    row_widget->pack_start(*handle, Gtk::PACK_SHRINK);
    row_widget->reorder_child(*handle, 0);

    register_drag_handle(*handle, *row_widget);
    Gtk::ListBox::add(*row_widget);
    // Add a separator after each item to indicate drop position.
    Gtk::ListBox::add(
        *Gtk::make_managed<Gtk::Separator>(Gtk::ORIENTATION_HORIZONTAL));
}

void DraggableListBox::register_drag_handle(Gtk::Widget& handle,
                                            Gtk::Widget& row_widget) {
    std::vector<Gtk::TargetEntry> targets = {
        Gtk::TargetEntry(kDragTarget, Gtk::TARGET_SAME_APP)};

    handle.drag_source_set(targets, Gdk::BUTTON1_MASK, Gdk::ACTION_MOVE);

    handle.signal_drag_begin().connect(
        sigc::bind(sigc::mem_fun(*this, &DraggableListBox::on_row_drag_begin),
                   &row_widget));

    handle.signal_drag_data_get().connect(
        sigc::mem_fun(*this, &DraggableListBox::on_row_drag_data_get));
}

bool DraggableListBox::on_drag_motion(
    const Glib::RefPtr<Gdk::DragContext>& /*context*/, int /*x*/, int y,
    guint /*time*/) {
    auto* highlight_row = get_separator_drop_row(*this, y);

    if (highlight_row) {
        drag_highlight_row(*highlight_row);
    } else {
        drag_unhighlight_row();
    }
    return true;
}

void DraggableListBox::on_drag_leave(
    const Glib::RefPtr<Gdk::DragContext>& /*context*/, guint /*time*/) {
    drag_unhighlight_row();
}

bool DraggableListBox::on_drag_drop(
    const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time) {
    on_drag_motion(context, x, y, time);
    drag_get_data(context, kDragTarget, time);
    return true;
}

void DraggableListBox::on_drag_data_received(
    const Glib::RefPtr<Gdk::DragContext>& context, int /*x*/, int y,
    const Gtk::SelectionData& /*selection_data*/, guint /*info*/, guint time) {
    bool success = false;
    auto* drop_row = get_separator_drop_row(*this, y);

    if (dragged_row_ && drop_row) {
        auto* dragged_row = dragged_row_;
        int dragged_index = dragged_row->get_index();
        int drop_separator_index = drop_row->get_index();
        if (dragged_index % 2 == 0 || drop_separator_index % 2 != 0) {
            drag_unhighlight_row();
            context->drag_finish(false, false, time);
            dragged_row_ = nullptr;
            std::cerr << "Dragging failure: ListBox structure broken."
                      << std::endl;
            return;
        }

        auto* dragged_separator = get_row_at_index(dragged_index + 1);
        if (!is_separator_row(dragged_separator)) {
            drag_unhighlight_row();
            context->drag_finish(false, false, time);
            dragged_row_ = nullptr;
            std::cerr << "Dragging failure: dragged row is not followed by a "
                         "separator."
                      << std::endl;
            return;
        }

        int drop_index = drop_separator_index + 1;
        int dragged_separator_index = dragged_index + 1;

        if (dragged_index < drop_index) {
            drop_index -= 2;
        }

        if (drop_index != dragged_index) {
            dragged_row->reference();
            dragged_separator->reference();

            remove(*dragged_separator);
            remove(*dragged_row);

            Gtk::ListBox::insert(*dragged_row, drop_index);
            Gtk::ListBox::insert(*dragged_separator, drop_index + 1);
            dragged_row->show();
            dragged_separator->show();

            dragged_row->unreference();
            dragged_separator->unreference();
            success = true;
        }
    }

    drag_unhighlight_row();
    context->drag_finish(success, false, time);
    dragged_row_ = nullptr;
}

void DraggableListBox::on_row_drag_begin(
    const Glib::RefPtr<Gdk::DragContext>& /*context*/,
    Gtk::Widget* row_widget) {
    auto* parent = row_widget ? row_widget->get_parent() : nullptr;
    dragged_row_ = dynamic_cast<Gtk::ListBoxRow*>(parent);
}

void DraggableListBox::on_row_drag_data_get(
    const Glib::RefPtr<Gdk::DragContext>& /*context*/,
    Gtk::SelectionData& selection_data, guint /*info*/, guint /*time*/) {
    const guint8 payload[] = {'1'};
    selection_data.set(selection_data.get_target(), 8, payload, 1);
}

}  // namespace ff::widgets

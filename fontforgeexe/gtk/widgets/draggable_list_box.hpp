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
#pragma once

#include <gtkmm.h>

namespace ff::widgets {

class DraggableListBox : public Gtk::ListBox {
 public:
    DraggableListBox();

    void register_drag_handle(Gtk::Widget& handle, Gtk::Widget& row_widget);

 private:
    Gtk::ListBoxRow* dragged_row_ = nullptr;

    bool on_drag_motion(const Glib::RefPtr<Gdk::DragContext>& /*context*/,
                        int /*x*/, int y, guint /*time*/);
    void on_drag_leave(const Glib::RefPtr<Gdk::DragContext>& /*context*/,
                       guint /*time*/);
    bool on_drag_drop(const Glib::RefPtr<Gdk::DragContext>& context, int /*x*/,
                      int y, guint time);
    void on_drag_data_received(const Glib::RefPtr<Gdk::DragContext>& context,
                               int /*x*/, int y,
                               const Gtk::SelectionData& /*selection_data*/,
                               guint /*info*/, guint time);

    void on_row_drag_begin(const Glib::RefPtr<Gdk::DragContext>& /*context*/,
                           Gtk::Widget* row_widget);
    void on_row_drag_data_get(const Glib::RefPtr<Gdk::DragContext>& /*context*/,
                              Gtk::SelectionData& selection_data,
                              guint /*info*/, guint /*time*/);
};

}  // namespace ff::widgets

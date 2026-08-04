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

#include <map>
#include <gtkmm.h>

#include "../bitmap_view_shim.hpp"

namespace ff::views {

enum BVDevice {
    bvd_undefined = -1,
    bvd_mouse_btn1,
    bvd_mouse_ctrl_btn1,
    bvd_mouse_btn2,
    bvd_mouse_ctrl_btn2,
    bvd_stylus,
    bvd_ctrl_stylus,
    bvd_eraser
};

class ViewToolbar : public Gtk::VBox {
 public:
    ViewToolbar(const std::vector<BitmapViewTool>& bitmap_view_tools);

    sigc::signal<void, const BitmapViewTool&>& signal_tool_clicked() {
        return signal_tool_clicked_;
    }

    BVDevice find_device(bvtools tool_id) const;
    const BitmapViewTool& find_tool_definition(BVDevice device) const;
    const BitmapViewTool& find_tool_definition(bvtools tool_id) const;

 private:
    // This is a somewhat complicated mapping of tools to devices. In fact, it's
    // bijectional. Each device can have at most one tool assigned to it, and
    // each tool can be assigned to at most one device.
    // NOTE: This mapping is preserved throughout the session.
    static std::map<BVDevice, bvtools> tool_map_;

    std::vector<BitmapViewTool> bitmap_view_tools_;
    std::map<bvtools, Gtk::RadioToolButton*> tool_button_map_;
    std::vector<Glib::RefPtr<Gtk::GestureMultiPress>> gestures_;
    sigc::signal<void, const BitmapViewTool&> signal_tool_clicked_;

    Glib::RefPtr<Gtk::GestureMultiPress> create_tool_button_controller(
        Gtk::RadioToolButton& button) const;

    // Create a toolbar icon and overlay it with tiny device modifier icons,
    // e.g. a tool with device bvd_mouse_ctrl_btn1 will have a tiny "Control"
    // icon in its corner.
    Glib::RefPtr<Gdk::Pixbuf> make_tool_icon(const std::string& icon_name,
                                             BVDevice device);

    void on_tool_button_clicked(GdkEventButton* event,
                                const BitmapViewTool& tool_def);
};

BVDevice determine_device(GdkEventButton* event);

}  // namespace ff::views
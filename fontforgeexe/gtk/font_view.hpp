/* Copyright 2023 Maxim Iorsh <iorsh@users.sourceforge.net>
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
#pragma once

#include <gtkmm-3.0/gtkmm.h>

#include "c_context.h"
#include "menu_builder.hpp"

namespace FontViewNS {

class FontViewUiContext : public FF::UiContext {
public:
    FontViewUiContext(Gtk::Window* window, FVContext** p_fv_context);
    ~FontViewUiContext() { delete legacy_context; }

    FF::ActivateCB get_activate_cb(int mid) const override;
    FF::EnabledCB get_enabled_cb(int mid) const override;
    FF::CheckedCB get_checked_cb(int mid) const override;

    FF::ActivateCB get_activate_select_cb(int mid) const;

    Glib::RefPtr<Gtk::AccelGroup> get_accel_group() const override {
	return accel_group;
    }

    FVContext* get_legacy_context() const { return legacy_context; }
private:
    FVContext* legacy_context = nullptr;
    Glib::RefPtr<Gtk::AccelGroup> accel_group;
};

extern std::vector<FF::MenuBarInfo> top_menu;
extern std::vector<FF::MenuInfo> popup_menu;

Gtk::Window* create_view(FVContext** p_fv_context, int width, int height);

enum merge_type SelMergeType();

}
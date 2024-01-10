/* Copyright 2024 Maxim Iorsh <iorsh@users.sourceforge.net>

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

The name of the author may not be used to endorse or promote
products derived from this software without specific prior written
permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <gtkmm-3.0/gtkmm.h>

#include "c_context.h"
#include "utils.hpp"

namespace FF {

enum EnabledState : bool {
    Enabled = true,
    Disabled = false
};

enum CheckableState : bool {
    Checkable = true,
    NonCheckable = false
};

struct LabelInfo {
    L10nText text;
    std::string image_file;
    CheckableState checkable = NonCheckable;
    Glib::ustring accelerator; // See the Gtk::AccelKey constructor for the format
};

using ActivateCB = std::function<void(void)>;
using EnabledCB = std::function<bool(void)>;

struct MenuInfo {
    LabelInfo label;
    std::vector<MenuInfo> *sub_menu;

    // By design menu callbacks don't have any arguments, since it's impossible
    // to know in advance what input they might need. All input should be passed
    // by lambda capture.
    EnabledCB enabled;
    ActivateCB handler;	/* called on mouse release */

    int mid;

    bool is_separator() const { return label.text == Glib::ustring(); }
};

static const ActivateCB LegacyAction;
static const ActivateCB NoAction = [](){}; // NOOP callable action
static const EnabledCB LegacyCheck;
static const EnabledCB AlwaysEnabled = [](){ return true; };

static const MenuInfo kMenuSeparator = {{""}};

struct MenuBarInfo {
    LabelInfo label;
    std::vector<FF::MenuInfo> *sub_menu;
    int mid;
};

Gtk::Menu* build_menu(const std::vector<FF::MenuInfo>& info, FVContext* fv_context);

Gtk::MenuBar* build_menu_bar(const std::vector<FF::MenuBarInfo>& info, FVContext* fv_context);

}
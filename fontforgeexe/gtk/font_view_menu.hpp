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

#include <glib/gi18n.h>
#include <gtkmm-3.0/gtkmm.h>

#include "menu_builder.hpp"

namespace FontViewNS {

static const int MID_Cut = 2101;
static const int MID_OpenOutline	= 2701;

std::vector<FF::MenuInfo> popup_menu = {
    { { N_("New O_utline Window"), "", FF::Enabled, FF::NonCheckable, "<control>u" }, nullptr, [](){}, MID_OpenOutline },
    FF::kMenuSeparator,
    { { N_("Cu_t"), "editcut.png", FF::Enabled, FF::NonCheckable, "<control>t" }, nullptr, [](){}, MID_Cut },
};

}
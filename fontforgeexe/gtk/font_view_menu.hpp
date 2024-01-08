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
static const int MID_Copy = 2102;
static const int MID_Paste	= 2103;
static const int MID_Clear	= 2104;
static const int MID_CopyRef = 2107;
static const int MID_UnlinkRef = 2108;
static const int MID_CopyWidth = 2111;
static const int MID_CopyFgToBg = 2115;
static const int MID_CharInfo	= 2201;
static const int MID_Transform = 2202;
static const int MID_Stroke = 2203;
static const int MID_Correct = 2206;
static const int MID_Round = 2213;
static const int MID_AutoHint = 2501;
static const int MID_Center = 2600;
static const int MID_SetWidth = 2602;
static const int MID_SetVWidth = 2605;
static const int MID_OpenOutline	= 2701;

std::vector<FF::MenuInfo> file_menu = {
    { { N_("_New"), "", FF::NonCheckable, "<control>u" }, nullptr, FF::AlwaysEnabled, FF::LegacyAction, MID_OpenOutline },
};

std::vector<FF::MenuInfo> hint_menu = {
    { { N_("Auto_Hint"), "hintsautohint", FF::NonCheckable, "<control><shift>H" }, nullptr, FF::LegacyCheck, FF::LegacyAction, MID_AutoHint },
};

std::vector<FF::MenuBarInfo> top_menu = {
    { { N_("_File") }, &file_menu, -1 },
    { { N_("_Edit") }, nullptr, -1 },
    { { N_("E_lement") }, nullptr, -1 },
#ifndef _NO_PYTHON
    { { N_("_Tools") }, nullptr, -1 },
#endif
    { { N_("H_ints") }, &hint_menu, -1 },
    { { N_("E_ncoding") }, nullptr, -1 },
    { { N_("_View") }, nullptr, -1 },
    { { N_("_Metrics") }, nullptr, -1 },
    { { N_("_CID") }, nullptr, -1 },
/* GT: Here (and following) MM means "MultiMaster" */
    { { N_("MM") }, nullptr, -1 },
    { { N_("_Window") }, nullptr, -1 },
    { { N_("_Help") }, nullptr, -1 },
};

std::vector<FF::MenuInfo> popup_menu = {
    { { N_("New O_utline Window"), "", FF::NonCheckable, "<control>u" }, nullptr, FF::AlwaysEnabled, FF::LegacyAction, MID_OpenOutline },
    FF::kMenuSeparator,
    { { N_("Cu_t"), "editcut", FF::NonCheckable, "<control>t" }, nullptr, FF::AlwaysEnabled, FF::LegacyAction, MID_Cut },
    { { N_("_Copy"), "editcopy", FF::NonCheckable, "<control>c" }, nullptr, FF::AlwaysEnabled, FF::LegacyAction, MID_Copy },
    { { N_("C_opy Reference"), "editcopyref", FF::NonCheckable, "<control>o" }, nullptr, FF::AlwaysEnabled, FF::LegacyAction, MID_CopyRef },
    { { N_("Copy _Width"), "editcopywidth", FF::NonCheckable, "<control>w" }, nullptr, FF::AlwaysEnabled, FF::LegacyAction, MID_CopyWidth },
    { { N_("_Paste"), "editpaste", FF::NonCheckable, "<control>p" }, nullptr, FF::AlwaysEnabled, FF::LegacyAction, MID_Paste },
    { { N_("C_lear"), "editclear", FF::NonCheckable, "" }, nullptr, FF::AlwaysEnabled, FF::LegacyAction, MID_Clear },
    { { N_("Copy _Fg To Bg"), "editcopyfg2bg", FF::NonCheckable, "<control><shift>F" }, nullptr, FF::AlwaysEnabled, FF::LegacyAction, MID_CopyFgToBg },
    { { N_("U_nlink Reference"), "editunlink", FF::NonCheckable, "<control>u" }, nullptr, FF::AlwaysEnabled, FF::LegacyAction, MID_UnlinkRef },
    FF::kMenuSeparator,
    { { N_("Glyph _Info..."), "elementglyphinfo", FF::NonCheckable, "<control>i" }, nullptr, FF::AlwaysEnabled, FF::LegacyAction, MID_CharInfo },
    { { N_("_Transform..."), "elementtransform", FF::NonCheckable, "<control>t" }, nullptr, FF::AlwaysEnabled, FF::LegacyAction, MID_Transform },
    { { N_("_Expand Stroke..."), "elementexpandstroke", FF::NonCheckable, "<control><shift>E" }, nullptr, FF::AlwaysEnabled, FF::LegacyAction, MID_Stroke },
    { { N_("To _Int"), "elementround", FF::NonCheckable, "<control><shift>I" }, nullptr, FF::AlwaysEnabled, FF::LegacyAction, MID_Round },
    { { N_("_Correct Direction"), "elementcorrectdir", FF::NonCheckable, "<control><shift>D" }, nullptr, FF::AlwaysEnabled, FF::LegacyAction, MID_Correct },
    FF::kMenuSeparator,
    { { N_("Auto_Hint"), "hintsautohint", FF::NonCheckable, "<control>h" }, nullptr, FF::AlwaysEnabled, FF::LegacyAction, MID_AutoHint },
    FF::kMenuSeparator,
    { { N_("_Center in Width"), "metricscenter", FF::NonCheckable, "<control>c" }, nullptr, FF::AlwaysEnabled, FF::LegacyAction, MID_Center },
    { { N_("Set _Width..."), "metricssetwidth", FF::NonCheckable, "<control><shift>W" }, nullptr, FF::AlwaysEnabled, FF::LegacyAction, MID_SetWidth },
    { { N_("Set _Vertical Advance..."), "metricssetvwidth", FF::NonCheckable, "<control><shift>V" }, nullptr, FF::AlwaysEnabled, FF::LegacyAction, MID_SetVWidth },
};

}
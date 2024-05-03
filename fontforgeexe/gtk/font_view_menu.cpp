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

#include <glib/gi18n.h>
#include <gtkmm-3.0/gtkmm.h>

#include "common_menus.hpp"
#include "font_view.hpp"
#include "menu_builder.hpp"

namespace FontViewNS {

static const int MID_24	= 2001;
static const int MID_36	= 2002;
static const int MID_48	= 2004;
static const int MID_72	= 2014;
static const int MID_96	= 2015;
static const int MID_128 = 2018;
static const int MID_AntiAlias = 2005;
static const int MID_Next = 2006;
static const int MID_Prev = 2007;
static const int MID_NextDef = 2012;
static const int MID_PrevDef = 2013;
static const int MID_ShowHMetrics = 2016;
static const int MID_ShowVMetrics = 2017;
static const int MID_Ligatures = 2020;
static const int MID_KernPairs = 2021;
static const int MID_AnchorPairs = 2022;
static const int MID_FitToBbox = 2023;
static const int MID_DisplaySubs = 2024;
static const int MID_32x8 = 2025;
static const int MID_16x4 = 2026;
static const int MID_8x2 = 2027;
static const int MID_BitmapMag = 2028;
static const int MID_GotoChar = 2030;
static const int MID_Show_ATT = 2031;

static const int MID_Cut = 2101;
static const int MID_Copy = 2102;
static const int MID_Paste	= 2103;
static const int MID_Clear	= 2104;
static const int MID_CopyRef = 2107;
static const int MID_UnlinkRef = 2108;
static const int MID_CopyWidth = 2111;
static const int MID_CopyFgToBg = 2115;

static const int MID_FontInfo = 2200;
static const int MID_CharInfo = 2201;
static const int MID_Transform = 2202;
static const int MID_Stroke = 2203;
static const int MID_RmOverlap = 2204;
static const int MID_Simplify = 2205;
static const int MID_Correct = 2206;
static const int MID_AvailBitmaps = 2210;
static const int MID_RegenBitmaps = 2211;
static const int MID_Autotrace = 2212;
static const int MID_Round = 2213;
static const int MID_MergeFonts = 2214;
static const int MID_InterpolateFonts = 2215;
static const int MID_FindProblems = 2216;
static const int MID_Embolden = 2217;
static const int MID_Condense = 2218;
static const int MID_AddExtrema = 2224;
static const int MID_CleanupGlyph = 2225;
static const int MID_NLTransform = 2228;
static const int MID_Intersection = 2229;
static const int MID_FindInter = 2230;
static const int MID_SimplifyMore = 2233;
static const int MID_POV = 2236;
static const int MID_StrikeInfo = 2238;
static const int MID_FontCompare = 2239;
static const int MID_CanonicalStart = 2242;
static const int MID_CanonicalContours = 2243;
static const int MID_RemoveBitmaps = 2244;
static const int MID_Validate = 2245;
static const int MID_MassRename = 2246;
static const int MID_Italic = 2247;
static const int MID_SmallCaps = 2248;
static const int MID_SubSup = 2249;
static const int MID_ChangeXHeight = 2250;
static const int MID_ChangeGlyph = 2251;
static const int MID_SetColor = 2252;
static const int MID_SetExtremumBound = 2253;
static const int MID_AddInflections = 2256;
static const int MID_Balance = 2257;
static const int MID_Harmonize = 2258;
static const int MID_LayersCompare = 2259;
static const int MID_MathInfo = 2260;
static const int MID_HorBaselines = 2261;
static const int MID_VertBaselines = 2262;
static const int MID_Justification = 2263;
static const int MID_Oblique = 2264;
static const int MID_Inline = 2265;
static const int MID_Outline = 2266;
static const int MID_Shadow = 2267;
static const int MID_Wireframe = 2268;

static const int MID_AutoHint = 2501;
static const int MID_ClearHints = 2502;
static const int MID_AutoInstr = 2504;
static const int MID_EditInstructions = 2505;
static const int MID_Editfpgm = 2506;
static const int MID_Editprep = 2507;
static const int MID_ClearInstrs = 2508;
static const int MID_HStemHist = 2509;
static const int MID_VStemHist = 2510;
static const int MID_BlueValuesHist = 2511;
static const int MID_Editcvt = 2512;
static const int MID_HintSubsPt = 2513;
static const int MID_AutoCounter = 2514;
static const int MID_DontAutoHint = 2515;
static const int MID_RmInstrTables = 2516;
static const int MID_Editmaxp = 2517;
static const int MID_Deltas = 2518;

static const int MID_Center = 2600;
static const int MID_SetWidth = 2602;
static const int MID_SetVWidth = 2605;
static const int MID_OpenOutline = 2701;

static const int MID_Reencode = 2830;
static const int MID_ForceReencode = 2831;
static const int MID_AddUnencoded = 2832;
static const int MID_RemoveUnused = 2833;
static const int MID_DetachGlyphs = 2834;
static const int MID_DetachAndRemoveGlyphs = 2835;
static const int MID_LoadEncoding = 2836;
static const int MID_MakeFromFont = 2837;
static const int MID_RemoveEncoding = 2838;
static const int MID_DisplayByGroups = 2839;
static const int MID_Compact = 2840;
static const int MID_SaveNamelist = 2841;
static const int MID_RenameGlyphs = 2842;
static const int MID_NameGlyphs = 2843;
static const int MID_AddEncoding = 2850;
static const int MID_DefineGroups = 2851;
static const int MID_LoadNameList = 2852;

static const int MIDSERIES_LabelGlyph = 10000;

std::vector<FF::MenuInfo> encodings(FVContext* fv_context,
                                    void (*encoding_action)(FontView*, const char*),
                                    FF::RadioGroup group) {
    EncodingMenuData* encoding_data_array = nullptr;
    int n_encodings = fv_context->collect_encoding_data(fv_context->fv, &encoding_data_array);
    std::vector<FF::MenuInfo> info_arr;

    for (int i = 0; i < n_encodings; ++i) {
        const EncodingMenuData& encoding_data = encoding_data_array[i];

        if (encoding_data.enc_name == nullptr) {
            info_arr.push_back(FF::kMenuSeparator);
            continue;
        }

        FF::ActivateCB action = [encoding_action, fv=fv_context->fv, enc_name=encoding_data.enc_name](const FF::UiContext&){
            encoding_action(fv, enc_name);
        };
        FF::CheckedCB checker = [cb=fv_context->current_encoding, fv=fv_context->fv, enc_name=encoding_data.enc_name](const FF::UiContext&){
            return cb(fv, enc_name);
        };
        FF::MenuInfo info{ { encoding_data.label, group, "" }, nullptr, { FF::AlwaysEnabled, checker, action }, 0 };
        info_arr.push_back(info);
    }
    return info_arr;
}

std::vector<FF::MenuInfo> encoding_reencode(const FF::UiContext& ui_context) {
    const FontViewUiContext& fv_ui_context = static_cast<const FontViewUiContext&>(ui_context);
    FVContext* fv_context = fv_ui_context.get_legacy_context();

    return encodings(fv_context, fv_context->change_encoding, FF::Encoding);
}

std::vector<FF::MenuInfo> encoding_force_encoding(const FF::UiContext& ui_context) {
    const FontViewUiContext& fv_ui_context = static_cast<const FontViewUiContext&>(ui_context);
    FVContext* fv_context = fv_ui_context.get_legacy_context();

    return encodings(fv_context, fv_context->force_encoding, FF::ForcedEncoding);
}

std::vector<FF::MenuInfo> view_menu_bitmaps(const FF::UiContext& ui_context) {
    const FontViewUiContext& fv_ui_context = static_cast<const FontViewUiContext&>(ui_context);
    FVContext* fv_context = fv_ui_context.get_legacy_context();

    BitmapMenuData* bitmap_data_array = nullptr;
    int n_bitmaps = fv_context->collect_bitmap_data(fv_context->fv, &bitmap_data_array);
    std::vector<FF::MenuInfo> info_arr;

    for (int i = 0; i < n_bitmaps; ++i) {
        const BitmapMenuData& bitmap_data = bitmap_data_array[i];
        char buffer[50];

	if (bitmap_data.depth == 1)
	    sprintf(buffer, _("%d pixel bitmap"), bitmap_data.pixelsize);
	else
	    sprintf(buffer, _("%d@%d pixel bitmap"),
                    bitmap_data.pixelsize, bitmap_data.depth);

        FF::ActivateCB action = [cb=fv_context->change_display_bitmap, fv=fv_context->fv, bdf=bitmap_data.bdf](const FF::UiContext&){
            cb(fv, bdf);
        };
        FF::CheckedCB checker = [cb=fv_context->current_display_bitmap, fv=fv_context->fv, bdf=bitmap_data.bdf](const FF::UiContext&){
            return cb(fv, bdf);
        };
        FF::MenuInfo info{ { buffer, FF::CellPixelView, "" }, nullptr, { FF::AlwaysEnabled, checker, action }, 0 };
        info_arr.push_back(info);
    }
    return info_arr;
}

std::vector<FF::MenuInfo> view_menu_layers(const FF::UiContext& ui_context) {
    const FontViewUiContext& fv_ui_context = static_cast<const FontViewUiContext&>(ui_context);
    FVContext* fv_context = fv_ui_context.get_legacy_context();

    LayerMenuData* layer_data_array = nullptr;
    int n_layers = fv_context->collect_layer_data(fv_context->fv, &layer_data_array);
    std::vector<FF::MenuInfo> info_arr;

    for (int i = 0; i < n_layers; ++i) {
        const LayerMenuData& layer_data = layer_data_array[i];

        FF::ActivateCB action = [cb=fv_context->change_display_layer, fv=fv_context->fv, ly=layer_data.index](const FF::UiContext&){
            cb(fv, ly);
        };
        FF::CheckedCB checker = [cb=fv_context->current_display_layer, fv=fv_context->fv, ly=layer_data.index](const FF::UiContext&){
            return cb(fv, ly);
        };
        FF::MenuInfo info{ { layer_data.label, FF::ActiveLayer, "" }, nullptr, { FF::AlwaysEnabled, checker, action }, 0 };
        info_arr.push_back(info);
    }
    return info_arr;
}

std::vector<FF::MenuInfo> view_menu_anchors(const FF::UiContext& ui_context) {
    const FontViewUiContext& fv_ui_context = static_cast<const FontViewUiContext&>(ui_context);
    FVContext* fv_context = fv_ui_context.get_legacy_context();

    AnchorMenuData* anchor_data_array = nullptr;
    int n_anchors = fv_context->collect_anchor_data(fv_context->fv, &anchor_data_array);

    std::vector<FF::MenuInfo> info_arr;

    // Special item for all anchors
    FF::ActivateCB action_all = [cb=fv_context->show_anchor_pair, fv=fv_context->fv](const FF::UiContext&){
        cb(fv, (AnchorClass*)-1);
    };
    info_arr.push_back({ { N_("All"), FF::NonCheckable, "" }, nullptr, { FF::AlwaysEnabled, FF::NotCheckable, action_all }, 0 });
    info_arr.push_back(FF::kMenuSeparator);

    for (int i = 0; i < n_anchors; ++i) {
        const AnchorMenuData& anchor_data = anchor_data_array[i];

        FF::ActivateCB action = [cb=fv_context->show_anchor_pair, fv=fv_context->fv, ac=anchor_data.ac](const FF::UiContext&){
            cb(fv, ac);
        };
        FF::MenuInfo info{ { anchor_data.label, FF::NonCheckable, "" }, nullptr, { FF::AlwaysEnabled, FF::NotCheckable, action }, 0 };
        info_arr.push_back(info);
    }
    return info_arr;
}

void run_autotrace(const FF::UiContext& ui_context) {
    const FontViewUiContext& fv_ui_context = static_cast<const FontViewUiContext&>(ui_context);
    FVContext* fv_context = fv_ui_context.get_legacy_context();
    Gtk::Widget* drawing_area = gtk_find_child(ui_context.window_, "CharGrid");

    auto old_cursor = set_cursor(ui_context.window_, "wait");
    auto old_cursor_da = set_cursor(drawing_area, "wait");

    bool shift_pressed = gtk_get_keyboard_state() & Gdk::ModifierType::SHIFT_MASK;
    fv_context->run_autotrace(fv_context->fv, shift_pressed);

    unset_cursor(ui_context.window_, old_cursor);
    unset_cursor(drawing_area, old_cursor_da);
}

std::vector<FF::MenuInfo> file_menu = {
    { { N_("_New"), FF::NonCheckable, "<control>u" }, nullptr, FF::LegacyCallbacks, MID_OpenOutline },
};

//////////////////////////////// ELEMENT MENUS ////////////////////////////////////////

std::vector<FF::MenuInfo> other_info_menu = {
    { { N_("_MATH Info..."), "elementmathinfo", "" }, nullptr, FF::LegacyCallbacks, MID_MathInfo },
    { { N_("_BDF Info..."), "elementbdfinfo", "" }, nullptr, FF::LegacyCallbacks, MID_StrikeInfo },
    { { N_("_Horizontal Baselines..."), "elementhbaselines", "" }, nullptr, FF::LegacyCallbacks, MID_HorBaselines },
    { { N_("_Vertical Baselines..."), "elementvbaselines", "" }, nullptr, FF::LegacyCallbacks, MID_VertBaselines },
    { { N_("_Justification..."), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_Justification },
    { { N_("Show _Dependent"), "elementshowdep", "" }, nullptr/*&show_dependent_menu*/, FF::SubMenuCallbacks, 0 },
    { { N_("Mass Glyph _Rename..."), "elementrenameglyph", "" }, nullptr, FF::LegacyCallbacks, MID_MassRename },
    { { N_("Set _Color"), FF::NonCheckable, "" }, nullptr/*&set_color_menu*/, { FF::LegacyEnabled, FF::NotCheckable, FF::NoAction }, MID_SetColor },
};

std::vector<FF::MenuInfo> validation_menu = {
    { { N_("Find Pr_oblems..."), "elementfindprobs", "<control>E" }, nullptr, FF::LegacyCallbacks, MID_FindProblems },
    { { N_("_Validate..."), "elementvalidate", "" }, nullptr, FF::LegacyCallbacks, MID_Validate },
    FF::kMenuSeparator,
    { { N_("Set E_xtremum Bound..."), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_SetExtremumBound },
};

std::vector<FF::MenuInfo> style_menu = {
    { { N_("Change _Weight..."), "styleschangeweight", "<control><shift>exclam" }, nullptr, FF::LegacyCallbacks, MID_Embolden },
    { { N_("_Italic..."), "stylesitalic", "" }, nullptr, FF::LegacyCallbacks, MID_Italic },
    { { N_("Obli_que..."), "stylesoblique", "" }, nullptr, FF::LegacyCallbacks, MID_Oblique },
    { { N_("_Condense/Extend..."), "stylesextendcondense", "" }, nullptr, FF::LegacyCallbacks, MID_Condense },
    { { N_("Change _X-Height..."), "styleschangexheight", "" }, nullptr, FF::LegacyCallbacks, MID_ChangeXHeight },
    { { N_("Change _Glyph..."), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_ChangeGlyph },
    FF::kMenuSeparator,
    { { N_("Add _Small Capitals..."), "stylessmallcaps", "" }, nullptr, FF::LegacyCallbacks, MID_SmallCaps },
    { { N_("Add Subscripts/Superscripts..."), "stylessubsuper", "" }, nullptr, FF::LegacyCallbacks, MID_SubSup },
    FF::kMenuSeparator,
    { { N_("In_line..."), "stylesinline", "" }, nullptr, FF::LegacyCallbacks, MID_Inline },
    { { N_("_Outline..."), "stylesoutline", "" }, nullptr, FF::LegacyCallbacks, MID_Outline },
    { { N_("S_hadow..."), "stylesshadow", "" }, nullptr, FF::LegacyCallbacks, MID_Shadow },
    { { N_("_Wireframe..."), "styleswireframe", "" }, nullptr, FF::LegacyCallbacks, MID_Wireframe },
};

std::vector<FF::MenuInfo> transformations_menu = {
    { { N_("_Transform..."), "elementtransform", "<control>backslash" }, nullptr, FF::LegacyCallbacks, MID_Transform },
    { { N_("_Point of View Projection..."), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_POV },
    { { N_("_Non Linear Transform..."), FF::NonCheckable, "<control><shift>colon" }, nullptr, FF::LegacyCallbacks, MID_NLTransform },
};

std::vector<FF::MenuInfo> overlap_menu = {
    { { N_("_Remove Overlap"), "overlaprm", "<control><shift>O" }, nullptr, FF::LegacyCallbacks, MID_RmOverlap },
    { { N_("_Intersect"), "overlapintersection", "" }, nullptr, FF::LegacyCallbacks, MID_Intersection },
    { { N_("_Find Intersections"), "overlapfindinter", "" }, nullptr, FF::LegacyCallbacks, MID_FindInter },
};

std::vector<FF::MenuInfo> simplify_menu = {
    { { N_("_Simplify"), "elementsimplify", "<control><shift>M" }, nullptr, FF::LegacyCallbacks, MID_Simplify },
    { { N_("Simplify More..."), FF::NonCheckable, "<alt><control><shift>M" }, nullptr, FF::LegacyCallbacks, MID_SimplifyMore },
    { { N_("Clea_nup Glyph"), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_CleanupGlyph },
    { { N_("Canonical Start _Point"), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_CanonicalStart },
    { { N_("Canonical _Contours"), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_CanonicalContours },
};

std::vector<FF::MenuInfo> element_menu = {
    { { N_("_Font Info..."), "elementfontinfo", "<control><shift>F" }, nullptr, FF::LegacyCallbacks, MID_FontInfo },
    { { N_("Glyph _Info..."), "elementglyphinfo", "<control>i" }, nullptr, FF::LegacyCallbacks, MID_CharInfo },
    { { N_("Other Info"), "elementotherinfo", "" }, &other_info_menu, FF::SubMenuCallbacks, 0 },
    { { N_("_Validation"), "elementvalidate", "" }, &validation_menu, FF::SubMenuCallbacks, 0 },
    FF::kMenuSeparator,
    { { N_("Bitm_ap Strikes Available..."), "elementbitmapsavail", "<control><shift>B" }, nullptr, FF::LegacyCallbacks, MID_AvailBitmaps },
    { { N_("Regenerate _Bitmap Glyphs..."), "elementregenbitmaps", "<control>B" }, nullptr, FF::LegacyCallbacks, MID_RegenBitmaps },
    { { N_("Remove Bitmap Glyphs..."), "elementremovebitmaps", "" }, nullptr, FF::LegacyCallbacks, MID_RemoveBitmaps },
    FF::kMenuSeparator,
    { { N_("St_yle"), "elementstyles", "" }, &style_menu, FF::SubMenuCallbacks, 0 },
    { { N_("_Transformations"), "elementtransform", "" }, &transformations_menu, FF::SubMenuCallbacks, 0 },
    { { N_("_Expand Stroke..."), "elementexpandstroke", "<control><shift>E" }, nullptr, FF::LegacyCallbacks, MID_Stroke },
#ifdef FONTFORGE_CONFIG_TILEPATH
    { { N_("Tile _Path..."), "elementtilepath", "" }, nullptr, FF::LegacyCallbacks, MID_TilePath },
    { { N_("Tile Pattern..."), "elementtilepattern", "" }, nullptr, FF::LegacyCallbacks, MID_TilePattern },
#endif
    { { N_("O_verlap"), "overlaprm", "" }, &overlap_menu, FF::SubMenuCallbacks, 0 },
    { { N_("_Simplify"), "elementsimplify", "" }, &simplify_menu, FF::SubMenuCallbacks, 0 },
    { { N_("Add E_xtrema"), "elementaddextrema", "<control><shift>X" }, nullptr, FF::LegacyCallbacks, MID_AddExtrema },
    { { N_("Add Points Of I_nflection"), "elementaddinflections", "<control><shift>Y" }, nullptr, FF::LegacyCallbacks, MID_AddInflections },
    { { N_("_Balance"), "elementbalance", "<control><shift>P" }, nullptr, FF::LegacyCallbacks, MID_Balance },
    { { N_("Harmoni_ze"), "elementharmonize", "<control><shift>Z" }, nullptr, FF::LegacyCallbacks, MID_Harmonize },
    { { N_("Roun_d"), "elementround", "" }, nullptr /*&round_menu*/, FF::SubMenuCallbacks, 0 },
    { { N_("Autot_race"), "elementautotrace", "<control><shift>T" }, nullptr, { FF::LegacyEnabled, FF::NotCheckable, run_autotrace }, MID_Autotrace },
    FF::kMenuSeparator,
    { { N_("_Correct Direction"), "elementcorrectdir", "<control><shift>D" }, nullptr, FF::LegacyCallbacks, MID_Correct },
    FF::kMenuSeparator,
    { { N_("B_uild"), "elementbuildaccent", "" }, nullptr /*&build_menu*/, FF::SubMenuCallbacks, 0 },
    FF::kMenuSeparator,
    { { N_("_Merge Fonts..."), "elementmergefonts", "" }, nullptr, FF::LegacyCallbacks, MID_MergeFonts },
    { { N_("Interpo_late Fonts..."), "elementinterpolatefonts", "" }, nullptr, FF::LegacyCallbacks, MID_InterpolateFonts },
    { { N_("Compare Fonts..."), "elementcomparefonts", "" }, nullptr, FF::LegacyCallbacks, MID_FontCompare },
    { { N_("Compare Layers..."), "elementcomparelayers", "" }, nullptr, FF::LegacyCallbacks, MID_LayersCompare },
};

/////////////////////////////////// TOOLS MENU ////////////////////////////////////////

std::vector<FF::MenuInfo> tools_menu = {
    FF::MenuInfo::CustomFVBlock(FF::python_tools),
};

std::vector<FF::MenuInfo> histograms_menu = {
    { { N_("_HStem"), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_HStemHist },
    { { N_("_VStem"), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_VStemHist },
    { { N_("BlueValues"), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_BlueValuesHist },
};

std::vector<FF::MenuInfo> hints_menu = {
    { { N_("Auto_Hint"), "hintsautohint", "<control><shift>H" }, nullptr, FF::LegacyCallbacks, MID_AutoHint },
    { { N_("Hint _Substitution Pts"), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_HintSubsPt },
    { { N_("Auto _Counter Hint"), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_AutoCounter },
    { { N_("_Don't AutoHint"), "hintsdontautohint", "" }, nullptr, FF::LegacyCallbacks, MID_DontAutoHint },
    FF::kMenuSeparator,
    { { N_("Auto_Instr"), FF::NonCheckable, "<control>T" }, nullptr, FF::LegacyCallbacks, MID_AutoInstr },
    { { N_("_Edit Instructions..."), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_EditInstructions },
    { { N_("Edit 'fpgm'..."), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_Editfpgm },
    { { N_("Edit 'prep'..."), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_Editprep },
    { { N_("Edit 'maxp'..."), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_Editmaxp },
    { { N_("Edit 'cvt '..."), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_Editcvt },
    { { N_("Remove Instr Tables"), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_RmInstrTables },
    { { N_("S_uggest Deltas..."), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_Deltas },
    FF::kMenuSeparator,
    { { N_("_Clear Hints"), "hintsclearvstems", "" }, nullptr, FF::LegacyCallbacks, MID_ClearHints },
    { { N_("Clear Instructions"), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_ClearInstrs },
    FF::kMenuSeparator,
    { { N_("Histograms"), FF::NonCheckable, "" }, &histograms_menu, FF::SubMenuCallbacks, 0 },
};

std::vector<FF::MenuInfo> reencode_menu = {
    FF::MenuInfo::CustomFVBlock(encoding_reencode),
};

std::vector<FF::MenuInfo> force_encoding_menu = {
    FF::MenuInfo::CustomFVBlock(encoding_force_encoding),
};

std::vector<FF::MenuInfo> encoding_menu = {
    { { N_("_Reencode"), FF::NonCheckable, "" }, &reencode_menu, FF::LegacyCallbacks, MID_Reencode },
    { { N_("_Compact (hide unused glyphs)"), FF::Checkable, "" }, nullptr, FF::LegacyCallbacks, MID_Compact },
    { { N_("_Force Encoding"), FF::NonCheckable, "" }, &force_encoding_menu, FF::LegacyCallbacks, MID_ForceReencode },
    FF::kMenuSeparator,
    { { N_("_Add Encoding Slots..."), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_AddUnencoded },
    { { N_("Remove _Unused Slots"), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_RemoveUnused },
    { { N_("_Detach Glyphs"), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_DetachGlyphs },
    { { N_("Detach & Remo_ve Glyphs..."), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_DetachAndRemoveGlyphs },
    FF::kMenuSeparator,
    { { N_("Add E_ncoding Name..."), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_AddEncoding },
    { { N_("_Load Encoding..."), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_LoadEncoding },
    { { N_("Ma_ke From Font..."), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_MakeFromFont },
    { { N_("Remove En_coding..."), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_RemoveEncoding },
    FF::kMenuSeparator,
    { { N_("Display By _Groups..."), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_DisplayByGroups },
    { { N_("D_efine Groups..."), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_DefineGroups },
    FF::kMenuSeparator,
    { { N_("_Save Namelist of Font..."), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_SaveNamelist },
    { { N_("L_oad Namelist..."), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_LoadNameList },
    { { N_("Rename Gl_yphs..."), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_RenameGlyphs },
    { { N_("Cre_ate Named Glyphs..."), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_NameGlyphs },
};

std::vector<FF::MenuInfo> layers_menu = {
    FF::MenuInfo::CustomFVBlock(view_menu_layers),
};

std::vector<FF::MenuInfo> anchor_pairs_menu = {
    FF::MenuInfo::CustomFVBlock(view_menu_anchors),
};

std::vector<FF::MenuInfo> combinations_menu = {
    { { N_("_Kern Pairs"), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_KernPairs },
    { { N_("_Anchored Pairs"), FF::NonCheckable, "" }, &anchor_pairs_menu, FF::LegacyCallbacks, MID_AnchorPairs },
    { { N_("_Ligatures"), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_Ligatures },
};

std::vector<FF::MenuInfo> label_glyph_menu = {
    { { N_("_Glyph Image"), FF::GlyphLabel, "" }, nullptr, FF::LegacyCallbacks, MIDSERIES_LabelGlyph + gl_glyph },
    { { N_("_Name"), FF::GlyphLabel, "" }, nullptr, FF::LegacyCallbacks, MIDSERIES_LabelGlyph + gl_name },
    { { N_("_Unicode"), FF::GlyphLabel, "" }, nullptr, FF::LegacyCallbacks, MIDSERIES_LabelGlyph + gl_unicode },
    { { N_("_Encoding Hex"), FF::GlyphLabel, "" }, nullptr, FF::LegacyCallbacks, MIDSERIES_LabelGlyph + gl_encoding },
};

std::vector<FF::MenuInfo> view_menu = {
    { { N_("_Next Glyph"), "viewnext", "<control>bracketright" }, nullptr, FF::LegacyCallbacks, MID_Next },
    { { N_("_Prev Glyph"), "viewprev", "<control>bracketleft" }, nullptr, FF::LegacyCallbacks, MID_Prev },
    { { N_("Next _Defined Glyph"), "viewnextdef", "<alt><control>bracketright" }, nullptr, FF::LegacyCallbacks, MID_NextDef },
    { { N_("Prev Defined Gl_yph"), "viewprevdef", "<alt><control>bracketleft" }, nullptr, FF::LegacyCallbacks, MID_PrevDef },
    { { N_("_Goto"), "viewgoto", "<control><shift>greater" }, nullptr, FF::LegacyCallbacks, MID_GotoChar },
    FF::kMenuSeparator,
    { { N_("_Layers"), "viewlayers", "" }, &layers_menu, FF::SubMenuCallbacks, 0 },
    FF::kMenuSeparator,
    { { N_("_Show ATT"), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_Show_ATT },
    { { N_("Display S_ubstitutions..."), FF::Checkable, "" }, nullptr, FF::LegacyCallbacks, MID_DisplaySubs },
    { { N_("Com_binations"), FF::NonCheckable, "" }, &combinations_menu, FF::SubMenuCallbacks, 0 },
    FF::kMenuSeparator,
    { { N_("Label Gl_yph By"), FF::NonCheckable, "" }, &label_glyph_menu, FF::SubMenuCallbacks, 0 },
    FF::kMenuSeparator,
    { { N_("S_how H. Metrics..."), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_ShowHMetrics },
    { { N_("Show _V. Metrics..."), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_ShowVMetrics },
    FF::kMenuSeparator,
    { { N_("32x8 cell window"), FF::CellWindowSize, "<control><shift>percent" }, nullptr, FF::LegacyCallbacks, MID_32x8 },
    { { N_("_16x4 cell window"), FF::CellWindowSize, "<control><shift>asciicircum" }, nullptr, FF::LegacyCallbacks, MID_16x4 },
    { { N_("_8x2  cell window"), FF::CellWindowSize, "<control><shift>asterisk" }, nullptr, FF::LegacyCallbacks, MID_8x2 },
    FF::kMenuSeparator,
    { { N_("_24 pixel outline"), FF::CellPixelView, "<control>2" }, nullptr, FF::LegacyCallbacks, MID_24 },
    { { N_("_36 pixel outline"), FF::CellPixelView, "<control>3" }, nullptr, FF::LegacyCallbacks, MID_36 },
    { { N_("_48 pixel outline"), FF::CellPixelView, "<control>4" }, nullptr, FF::LegacyCallbacks, MID_48 },
    { { N_("_72 pixel outline"), FF::CellPixelView, "<control>7" }, nullptr, FF::LegacyCallbacks, MID_72 },
    { { N_("_96 pixel outline"), FF::CellPixelView, "<control>9" }, nullptr, FF::LegacyCallbacks, MID_96 },
    { { N_("_128 pixel outline"), FF::CellPixelView, "<control>1" }, nullptr, FF::LegacyCallbacks, MID_128 },
    { { N_("_Anti Alias"), FF::Checkable, "<control>5" }, nullptr, FF::LegacyCallbacks, MID_AntiAlias },
    { { N_("_Fit to font bounding box"), FF::Checkable, "<control>6" }, nullptr, FF::LegacyCallbacks, MID_FitToBbox },
    FF::kMenuSeparator,
    { { N_("Bitmap _Magnification..."), FF::NonCheckable, "" }, nullptr, FF::LegacyCallbacks, MID_BitmapMag },
    FF::MenuInfo::CustomFVBlock(view_menu_bitmaps),
};

std::vector<FF::MenuBarInfo> top_menu = {
    { { N_("_File") }, &file_menu, -1 },
    { { N_("_Edit") }, nullptr, -1 },
    { { N_("E_lement") }, &element_menu, -1 },
#ifndef _NO_PYTHON
    { { N_("_Tools") }, &tools_menu, -1 },
#endif
    { { N_("H_ints") }, &hints_menu, -1 },
    { { N_("E_ncoding") }, &encoding_menu, -1 },
    { { N_("_View") }, &view_menu, -1 },
    { { N_("_Metrics") }, nullptr, -1 },
    { { N_("_CID") }, nullptr, -1 },
/* GT: Here (and following) MM means "MultiMaster" */
    { { N_("MM") }, nullptr, -1 },
    { { N_("_Window") }, nullptr, -1 },
    { { N_("_Help") }, nullptr, -1 },
};

std::vector<FF::MenuInfo> popup_menu = {
    { { N_("New O_utline Window"), FF::NonCheckable, "<control>u" }, nullptr, FF::LegacyCallbacks, MID_OpenOutline },
    FF::kMenuSeparator,
    { { N_("Cu_t"), "editcut", "<control>t" }, nullptr, FF::LegacyCallbacks, MID_Cut },
    { { N_("_Copy"), "editcopy", "<control>c" }, nullptr, FF::LegacyCallbacks, MID_Copy },
    { { N_("C_opy Reference"), "editcopyref", "<control>o" }, nullptr, FF::LegacyCallbacks, MID_CopyRef },
    { { N_("Copy _Width"), "editcopywidth", "<control>w" }, nullptr, FF::LegacyCallbacks, MID_CopyWidth },
    { { N_("_Paste"), "editpaste", "<control>p" }, nullptr, FF::LegacyCallbacks, MID_Paste },
    { { N_("C_lear"), "editclear", "" }, nullptr, FF::LegacyCallbacks, MID_Clear },
    { { N_("Copy _Fg To Bg"), "editcopyfg2bg", "<control><shift>F" }, nullptr, FF::LegacyCallbacks, MID_CopyFgToBg },
    { { N_("U_nlink Reference"), "editunlink", "<control>u" }, nullptr, FF::LegacyCallbacks, MID_UnlinkRef },
    FF::kMenuSeparator,
    { { N_("Glyph _Info..."), "elementglyphinfo", "<control>i" }, nullptr, FF::LegacyCallbacks, MID_CharInfo },
    { { N_("_Transform..."), "elementtransform", "<control>t" }, nullptr, FF::LegacyCallbacks, MID_Transform },
    { { N_("_Expand Stroke..."), "elementexpandstroke", "<control><shift>E" }, nullptr, FF::LegacyCallbacks, MID_Stroke },
    { { N_("To _Int"), "elementround", "<control><shift>I" }, nullptr, FF::LegacyCallbacks, MID_Round },
    { { N_("_Correct Direction"), "elementcorrectdir", "<control><shift>D" }, nullptr, FF::LegacyCallbacks, MID_Correct },
    FF::kMenuSeparator,
    { { N_("Auto_Hint"), "hintsautohint", "<control>h" }, nullptr, FF::LegacyCallbacks, MID_AutoHint },
    FF::kMenuSeparator,
    { { N_("_Center in Width"), "metricscenter", "<control>c" }, nullptr, FF::LegacyCallbacks, MID_Center },
    { { N_("Set _Width..."), "metricssetwidth", "<control><shift>W" }, nullptr, FF::LegacyCallbacks, MID_SetWidth },
    { { N_("Set _Vertical Advance..."), "metricssetvwidth", "<control><shift>V" }, nullptr, FF::LegacyCallbacks, MID_SetVWidth },
};

}
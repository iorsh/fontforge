/* Copyright (C) 2003-2012 by George Williams */
/*
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

#include <fontforge-config.h>

#include "autohint.h"
#include "dumppfa.h"
#include "ffglib.h"
extern "C" {
#include "fontforgeui.h"
}
#include "gkeysym.h"
#include "gresedit.h"
#include "psfont.h"
#include "splineutil.h"
#include "ustring.h"
#include "utype.h"

#include <math.h>
#include <cassert>

#include "gtk/show_histogram_shim.hpp"

/* This operations are designed to work on a single font. NOT a CID collection*/
/*  A CID collection must be treated one sub-font at a time */


typedef struct histdata {
    int low, high;
    std::vector<ff::dlg::HistogramBarRecord> hist;	/* array of high-low+1 elements */
} HistData;

static HistData HistFindBlues(SplineFont *sf,int layer, uint8_t *selected, EncMap *map) {
	std::map<int, ff::dlg::HistogramBarRecord> blues_map;
    HistData hist;

    for (int i=0; i<(selected==NULL?sf->glyphcnt:map->enccount); ++i ) {
    int top,bottom;
    SplineChar *sc;
    DBounds b;

    int gid = selected==NULL ? i : map->map[i];
	if ( gid!=-1 && (sc = sf->glyphs[gid])!=NULL &&
		sc->layers[ly_fore].splines!=NULL &&
		sc->layers[ly_fore].refs==NULL &&
		(selected==NULL || selected[i])) {
	    SplineCharLayerFindBounds(sc,layer,&b);
	    bottom = rint(b.miny);
	    top = rint(b.maxy);
	    if ( top==bottom )
	continue;

	blues_map[top].count++;
	blues_map[top].glyph_names.push_back(sc->name);

	blues_map[bottom].count++;
	blues_map[bottom].glyph_names.push_back(sc->name);
	}
    }
    if ( blues_map.empty() ) {		/* Found nothing */
	hist.low = hist.high = 0;
    }

    // Convert map to array for use in the histogram.
    hist.low = blues_map.begin()->first;
    hist.high = blues_map.rbegin()->first;
    hist.hist.resize(hist.high - hist.low + 1);
    for (const auto& [key, value] : blues_map) {
        hist.hist[key - hist.low] = value;
    }
return  hist ;
}

static HistData HistFindStemWidths(SplineFont *sf,int layer, uint8_t *selected,EncMap *map,int hor) {
	std::map<int, ff::dlg::HistogramBarRecord> blues_map;
    int i, gid, low,high, val;
    SplineChar *sc;
    HistData hist;
    struct hentry *h;
    StemInfo *stem;

    for ( i=0; i<(selected==NULL?sf->glyphcnt:map->enccount); ++i ) {
	gid = selected==NULL ? i : map->map[i];
	if ( gid!=-1 && (sc = sf->glyphs[gid])!=NULL &&
		sc->layers[ly_fore].splines!=NULL &&
		sc->layers[ly_fore].refs==NULL &&
		(selected==NULL || selected[i])) {
	    if ( autohint_before_generate && sc->changedsincelasthinted && !sc->manualhints )
		SplineCharAutoHint(sc,layer,NULL);
	    for ( stem = hor ? sc->hstem : sc->vstem ; stem!=NULL; stem = stem->next ) {
		if ( stem->ghost )
	    continue;
		val = rint(stem->width);
		if ( val<=0 )
		    val = -val;

	blues_map[val].count++;
	blues_map[val].glyph_names.push_back(sc->name);
    }}}

    if ( blues_map.empty() ) {		/* Found nothing */
	hist.low = hist.high = 0;
    }

    // Convert map to array for use in the histogram.
    hist.low = blues_map.begin()->first;
    hist.high = blues_map.rbegin()->first;
    hist.hist.resize(hist.high - hist.low + 1);
    for (const auto& [key, value] : blues_map)
        hist.hist[key - hist.low] = value;

return  hist ;
}

static HistData HistFindHStemWidths(SplineFont *sf,int layer, uint8_t *selected,EncMap *map) {
return( HistFindStemWidths(sf,layer,selected,map,true) );
}

static HistData HistFindVStemWidths(SplineFont *sf,int layer, uint8_t *selected,EncMap *map) {
return( HistFindStemWidths(sf,layer,selected,map,false) );
}
	
static void HistSet(SplineFont *sf, struct psdict *private_dict, const ff::dlg::UiStrings& ui_strings, const ff::dlg::PrivateDictValues& result) {
	if ((result.primary.empty() || result.primary == "[]") && 
	    (result.secondary.empty() || result.secondary == "[]") &&
	    private_dict == NULL)
            return;

        if ( private_dict==NULL ) {
	    sf->private_dict = private_dict = (psdict *)calloc(1,sizeof(struct psdict));
	    private_dict->cnt = 10;
	    private_dict->keys = (char **)calloc(10,sizeof(char *));
	    private_dict->values = (char **)calloc(10,sizeof(char *));
        }
        PSDictChangeEntry(private_dict,ui_strings.primary_label.c_str(),result.primary.c_str());
        PSDictChangeEntry(private_dict,ui_strings.secondary_label.c_str(),result.secondary.c_str());
}

static bool CheckSmallSelection(uint8_t *selected,EncMap *map,SplineFont *sf) {
    int i, cnt, tot;

    if ( selected==NULL )
        // All glyphs are considered selected, so no "small selection" warning.
        return( false );

    for ( i=cnt=tot=0; i<map->enccount; ++i ) {
	int gid = map->map[i];
	if ( gid!=-1 && sf->glyphs[gid]!=NULL ) {
	    ++tot;
	    if ( selected[i] )
		++cnt;
	}
    }
    return ( (cnt==1 && tot>1) || (cnt<8 && tot>30) );
}

void SFHistogram(GWindow parent, SplineFont *sf,int layer, struct psdict *private_dict, uint8_t *selected,
	EncMap *map,enum hist_type which) {
    int i,j;
    const char *primary, *secondary;
    HistData hist;

    if ( private_dict==NULL ) private_dict = sf->private_dict;
    switch ( which ) {
      case hist_hstem:
	hist = HistFindHStemWidths(sf,layer,selected,map);
      break;
      case hist_vstem:
	hist = HistFindVStemWidths(sf,layer,selected,map);
      break;
      case hist_blues:
	hist = HistFindBlues(sf,layer,selected,map);
      break;
    }

        ff::dlg::HistogramData dlg_data;
        dlg_data.type = which;
        dlg_data.lower_bound = hist.low;
	dlg_data.small_selection_warning = CheckSmallSelection(selected,map,sf);

	    const ff::dlg::UiStrings& ui_strings = ff::dlg::kHistogramUiStrings.at(dlg_data.type);

	if ( (j=PSDictFindEntry(private_dict,ui_strings.primary_label.c_str()))!=-1 )
	    dlg_data.initial_values.primary = private_dict->values[j];
	if ( (j=PSDictFindEntry(private_dict,ui_strings.secondary_label.c_str()))!=-1 )
	    dlg_data.initial_values.secondary = private_dict->values[j];

        for (int v = hist.low; v <= hist.high; ++v) {
            const ff::dlg::HistogramBarRecord& entry = hist.hist[v - hist.low];
            dlg_data.bars.push_back(entry);
        }

        std::optional<ff::dlg::PrivateDictValues> result = ff::dlg::show_histogram_dialog(parent, dlg_data);

	if (result.has_value())
	HistSet(sf, private_dict, ui_strings, result.value());
}

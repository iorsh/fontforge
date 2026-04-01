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

#include "gtk/show_histogram_shim.hpp"

/* This operations are designed to work on a single font. NOT a CID collection*/
/*  A CID collection must be treated one sub-font at a time */

GResFont histogram_font = GRESFONT_INIT("400 10pt " SANS_UI_FAMILIES);
Color histogram_graphcol = 0x2020ff;

struct hentry {
    int cnt, sum;
    int char_cnt, max;
    SplineChar **chars;
};

typedef struct histdata {
    int low, high;
    struct hentry *hist;	/* array of high-low+1 elements */
    int tot, max;
} HistData;

static void HistDataFree(HistData *h) {
    int i;

    for ( i=h->low; i<=h->high; ++i )
	free(h->hist[i-h->low].chars);
    free(h->hist);
    free(h);
}

static HistData *HistFindBlues(SplineFont *sf,int layer, uint8_t *selected, EncMap *map) {
    int i, gid, low,high, top,bottom;
    SplineChar *sc;
    DBounds b;
    HistData *hist;
    struct hentry *h;

    hist = (HistData *)calloc(1,sizeof(HistData));
    hist->hist = (struct hentry *)calloc(sf->ascent+sf->descent+1,sizeof(struct hentry));
    hist->low = sf->ascent; hist->high = -sf->descent;
    low = -sf->descent; high = sf->ascent;

    for ( i=0; i<(selected==NULL?sf->glyphcnt:map->enccount); ++i ) {
	gid = selected==NULL ? i : map->map[i];
	if ( gid!=-1 && (sc = sf->glyphs[gid])!=NULL &&
		sc->layers[ly_fore].splines!=NULL &&
		sc->layers[ly_fore].refs==NULL &&
		(selected==NULL || selected[i])) {
	    SplineCharLayerFindBounds(sc,layer,&b);
	    bottom = rint(b.miny);
	    top = rint(b.maxy);
	    if ( top==bottom )
	continue;
	    if ( top>hist->high ) {
		hist->high = top;
		if ( top>high ) {
		    hist->hist = (struct hentry *)realloc(hist->hist,(top+10-low)*sizeof(struct hentry));
		    memset(hist->hist + high-low+1,0,(top+10-high-1)*sizeof(struct hentry));
		    high = top+10 -1;
		}
	    }
	    ++ hist->hist[top-low].cnt;
	    if ( hist->hist[top-low].char_cnt >= hist->hist[top-low].max ) {
		if ( hist->hist[top-low].max==0 )
		    hist->hist[top-low].chars = (SplineChar **)malloc(10*sizeof(SplineChar *));
		else
		    hist->hist[top-low].chars = (SplineChar **)realloc(hist->hist[top-low].chars,(hist->hist[top-low].max+10)*sizeof(SplineChar *));
		hist->hist[top-low].max += 10;
	    }
	    hist->hist[top-low].chars[hist->hist[top-low].char_cnt++] = sc;

	    if ( bottom<hist->low ) {
		hist->low = bottom;
		if ( bottom<low ) {
		    h = (struct hentry *)calloc((high-bottom+10),sizeof( struct hentry ));
		    memcpy(h+low-(bottom-10+1),hist->hist,(high+1-low)*sizeof(struct hentry));
		    low = bottom-10+1;
		    free( hist->hist );
		    hist->hist = h;
		}
	    }
	    ++ hist->hist[bottom-low].cnt;
	    if ( hist->hist[bottom-low].char_cnt >= hist->hist[bottom-low].max ) {
		if ( hist->hist[bottom-low].max==0 )
		    hist->hist[bottom-low].chars = (SplineChar **)malloc(10*sizeof(SplineChar *));
		else
		    hist->hist[bottom-low].chars = (SplineChar **)realloc(hist->hist[bottom-low].chars,(hist->hist[bottom-low].max+10)*sizeof(SplineChar *));
		hist->hist[bottom-low].max += 10;
	    }
	    hist->hist[bottom-low].chars[hist->hist[bottom-low].char_cnt++] = sc;
	}
	hist->tot += 2;
    }
    if ( hist->low>hist->high ) {		/* Found nothing */
	hist->low = hist->high = 0;
    }
    if ( low!=hist->low || high!=hist->high ) {
	h = (struct hentry *)malloc((hist->high-hist->low+1)*sizeof(struct hentry));
	memcpy(h,hist->hist + hist->low-low,(hist->high-hist->low+1)*sizeof(struct hentry));
	free(hist->hist);
	hist->hist = h;
    }
return( hist );
}

static HistData *HistFindStemWidths(SplineFont *sf,int layer, uint8_t *selected,EncMap *map,int hor) {
    int i, gid, low,high, val;
    SplineChar *sc;
    HistData *hist;
    struct hentry *h;
    StemInfo *stem;

    hist = (HistData *)calloc(1,sizeof(HistData));
    hist->hist = (struct hentry *)calloc(sf->ascent+sf->descent+1,sizeof(struct hentry));
    hist->low = sf->ascent+sf->descent;
    low = 0; high = sf->ascent+sf->descent;

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
		if ( val>hist->high ) {
		    hist->high = val;
		    if ( val>high ) {
			hist->hist = (struct hentry *)realloc(hist->hist,(val+10-low)*sizeof(struct hentry));
			memset(hist->hist + high-low+1,0,(val+10-high-1)*sizeof(struct hentry));
			high = val+10 -1;
		    }
		}
		if ( val<hist->low )
		    hist->low = val;
		++ hist->hist[val-low].cnt;
		if ( hist->hist[val-low].char_cnt==0 ||
			hist->hist[val-low].chars[hist->hist[val-low].char_cnt-1]!=sc ) {
		    if ( hist->hist[val-low].char_cnt >= hist->hist[val-low].max ) {
			if ( hist->hist[val-low].max==0 )
			    hist->hist[val-low].chars = (SplineChar **)malloc(10*sizeof(SplineChar *));
			else
			    hist->hist[val-low].chars = (SplineChar **)realloc(hist->hist[val-low].chars,(hist->hist[val-low].max+10)*sizeof(SplineChar *));
			hist->hist[val-low].max += 10;
		    }
		    hist->hist[val-low].chars[hist->hist[val-low].char_cnt++] = sc;
		}
		++ hist->tot;
	    }
	}
    }
    if ( hist->low>hist->high ) {		/* Found nothing */
	hist->low = hist->high = 0;
    }
    if ( low!=hist->low || high!=hist->high ) {
	h = (struct hentry *)malloc((hist->high-hist->low+1)*sizeof(struct hentry));
	memcpy(h,hist->hist + hist->low-low,(hist->high-hist->low+1)*sizeof(struct hentry));
	free(hist->hist);
	hist->hist = h;
    }
return( hist );
}

static HistData *HistFindHStemWidths(SplineFont *sf,int layer, uint8_t *selected,EncMap *map) {
return( HistFindStemWidths(sf,layer,selected,map,true) );
}

static HistData *HistFindVStemWidths(SplineFont *sf,int layer, uint8_t *selected,EncMap *map) {
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
    HistData *hist;

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
        dlg_data.lower_bound = hist->low;
	dlg_data.small_selection_warning = CheckSmallSelection(selected,map,sf);

	    const ff::dlg::UiStrings& ui_strings = ff::dlg::kHistogramUiStrings.at(dlg_data.type);

	if ( (j=PSDictFindEntry(private_dict,ui_strings.primary_label.c_str()))!=-1 )
	    dlg_data.initial_values.primary = private_dict->values[j];
	if ( (j=PSDictFindEntry(private_dict,ui_strings.secondary_label.c_str()))!=-1 )
	    dlg_data.initial_values.secondary = private_dict->values[j];

        for (int v = hist->low; v <= hist->high; ++v) {
            const hentry& entry = hist->hist[v - hist->low];
            dlg_data.bars.push_back({static_cast<unsigned int>(entry.cnt), {}});
	    for (int i = 0; i < entry.char_cnt; ++i) {
		dlg_data.bars.back().glyph_names.push_back(entry.chars[i]->name);
	    }
        }

        std::optional<ff::dlg::PrivateDictValues> result = ff::dlg::show_histogram_dialog(parent, dlg_data);

	if (result.has_value())
	HistSet(sf, private_dict, ui_strings, result.value());

    HistDataFree(hist);
}

/* Copyright 2024 Maxim Iorsh <iorsh@users.sourceforge.net>
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
#include "harfbuzz.hpp"

#include <algorithm>
#include <cassert>

extern "C" {
#include "gfile.h"
#include "splinechar.h"
#include "splinefont_enums.h"
#include "utype.h"
}

namespace ff::shapers {

HarfBuzzShaper::HarfBuzzShaper(std::shared_ptr<ShaperContext> context)
    : context_(context) {
    FILE* ttf_file = GFileTmpfile();

    _WriteTTFFont(
        ttf_file, context_->sf, ff_ttf, NULL, bf_ttf,
        ttf_flag_otmode | ttf_flag_oldkernmappedonly | ttf_flag_fake_map,
        context_->get_enc_map(context_->sf), ly_fore);

    // Calculate file length
    fseek(ttf_file, 0L, SEEK_END);
    long bufsize = ftell(ttf_file);
    fseek(ttf_file, 0L, SEEK_SET);

    // Read the entire file into memory
    blob = (char*)malloc(sizeof(char) * (bufsize + 1));
    size_t blob_size = fread(blob, sizeof(char), bufsize, ttf_file);

    hb_ttf_blob =
        hb_blob_create(blob, blob_size, HB_MEMORY_MODE_WRITABLE, NULL, NULL);

    hb_ttf_face = hb_face_create(hb_ttf_blob, 0);

    hb_ttf_font = hb_font_create(hb_ttf_face);

    fclose(ttf_file);
}

HarfBuzzShaper::~HarfBuzzShaper() {
    hb_font_destroy(hb_ttf_font);
    hb_face_destroy(hb_ttf_face);
    hb_blob_destroy(hb_ttf_blob);
    free(blob);
}

SplineChar** HarfBuzzShaper::extract_shaped_data(hb_buffer_t* hb_buffer) {
    unsigned int glyph_count;
    hb_glyph_info_t* glyph_info_arr =
        hb_buffer_get_glyph_infos(hb_buffer, &glyph_count);
    hb_glyph_position_t* glyph_pos_arr =
        hb_buffer_get_glyph_positions(hb_buffer, &glyph_count);

    // Return NULL-terminated raw C-style array of pointers
    SplineChar** glyphs_after_gpos =
        (SplineChar**)calloc(glyph_count + 1, sizeof(SplineChar*));

    // Adjust metrics buffer size
    metrics.resize(glyph_count + 1);

    // Process the glyphs and positions
    int total_x_advance = 0, total_y_advance = 0;
    for (int i = 0; i < glyph_count; ++i) {
        char glyph_name[64];
        hb_glyph_info_t& glyph_info = glyph_info_arr[i];
        hb_glyph_position_t& glyph_pos = glyph_pos_arr[i];

        // Warning: after the shaping glyph_info->codepoint is not a Unicode
        // point, but rather an internal glyph index. We can't use it in our
        // functions.
        hb_bool_t found =
            hb_font_get_glyph_name(hb_ttf_font, glyph_info.codepoint,
                                   glyph_name, sizeof(glyph_name) - 1);
        SplineChar* glyph_out =
            context_->get_glyph_by_name(context_->sf, -1, glyph_name);

        glyphs_after_gpos[i] = glyph_out;

        // Fill unscaled metrics in font units
        hb_position_t h_advance =
            hb_font_get_glyph_h_advance(hb_ttf_font, glyph_info.codepoint);

        hb_glyph_extents_t extents;
        hb_bool_t res = hb_font_get_glyph_extents(
            hb_ttf_font, glyph_info.codepoint, &extents);
        assert(res);

        metrics[i].dwidth = h_advance;
        metrics[i].dheight = glyph_out->vwidth;

        metrics[i].xoff = glyph_pos.x_offset;
        metrics[i].yoff = glyph_pos.y_offset;

        metrics[i].dx = total_x_advance;
        metrics[i].dy = total_y_advance;

        total_x_advance += glyph_pos.x_advance;
        total_y_advance += glyph_pos.y_advance;

        metrics[i].kernafter = 0;
        metrics[i].scaled = false;
    }

    // Fill the trailing empty object with auxiliaty data
    metrics.back().dx = total_x_advance;
    metrics.back().dy = total_y_advance;
    metrics.back().scaled = false;

    return glyphs_after_gpos;
}

std::vector<ShapeMetrics> HarfBuzzShaper::reverse_rtl_metrics(
    const std::vector<ShapeMetrics>& reverse_metrics) const {
    std::vector<ShapeMetrics> fixed_metrics(reverse_metrics.size());

    // Note: metrics contain a trailing element for C compatibility
    int glyph_count = reverse_metrics.size() - 1;
    int16_t total_x_advance = reverse_metrics.back().dx;
    int16_t total_y_advance = reverse_metrics.back().dy;

    for (int i = 0; i < glyph_count; ++i) {
        int rev_idx = glyph_count - i - 1;
        fixed_metrics[i].dwidth = reverse_metrics[rev_idx].dwidth;
        fixed_metrics[i].dheight = reverse_metrics[rev_idx].dheight;

        fixed_metrics[i].xoff = -reverse_metrics[rev_idx].xoff;
        fixed_metrics[i].yoff = reverse_metrics[rev_idx].yoff;

        fixed_metrics[i].dx = total_x_advance - reverse_metrics[rev_idx].dx -
                              reverse_metrics[rev_idx].dwidth;
        fixed_metrics[i].dy = total_y_advance - reverse_metrics[rev_idx].dy;
    }

    return fixed_metrics;
}

std::vector<int> HarfBuzzShaper::compute_kerning_deltas(
    hb_buffer_t* hb_buffer, struct opentype_str* ots_arr) {
    unsigned int glyph_count;
    hb_glyph_info_t* glyph_info_arr =
        hb_buffer_get_glyph_infos(hb_buffer, &glyph_count);

    // Retrieve the current kerning offsets and apply them manually if they
    // differ from their initial value. The initial value doesn't need
    // adjustment, since it is hopefully present in the generated font, so that
    // HarfBuzz takes it into account automatically.
    std::vector<int> kerning_deltas;
    for (int i = 0; i + 1 < glyph_count; ++i) {
        int kerning_offset = context_->get_kern_offset(ots_arr + i);
        if (kerning_offset == INVALID_KERN_OFFSET) {
            kerning_offset = 0;
        }

        // Keep initial kerning offsets when they are first encountered
        auto key = std::make_pair(glyph_info_arr[i].codepoint,
                                  glyph_info_arr[i + 1].codepoint);
        // Insert only if absent
        initial_kerning_.insert({key, kerning_offset});

        // Compute kerning deltas. Any existing kerning which was not manually
        // changed by the user should give zero delta.
        kerning_deltas.push_back(kerning_offset - initial_kerning_[key]);
    }

    return kerning_deltas;
}

std::vector<int> HarfBuzzShaper::compute_width_deltas(hb_buffer_t* hb_buffer,
                                                      SplineChar** glyphs) {
    unsigned int glyph_count;
    hb_glyph_info_t* glyph_info_arr =
        hb_buffer_get_glyph_infos(hb_buffer, &glyph_count);

    // Compute width deltas for glyphs which might have changed.
    std::vector<int> width_deltas;
    for (int i = 0; i < glyph_count; ++i) {
        int16_t width = glyphs[i]->width;

        // Keep initial widths when they are first encountered
        auto key = glyph_info_arr[i].codepoint;
        // Insert only if absent
        initial_width_.insert({key, width});

        // Compute kerning deltas. Any existing kerning which was not manually
        // changed by the user should give zero delta.
        width_deltas.push_back(width - initial_width_[key]);
    }

    return width_deltas;
}

struct opentype_str* HarfBuzzShaper::apply_features(
    SplineChar** glyphs, const std::vector<Tag>& feature_list, Tag script,
    Tag lang, int pixelsize) {
    std::vector<unichar_t> u_vec;
    for (size_t len = 0; glyphs[len] != NULL; ++len) {
        u_vec.push_back(
            (glyphs[len]->unicodeenc > 0)
                ? glyphs[len]->unicodeenc
                : context_->fake_unicode(context_->mv, glyphs[len]));
    }
    u_vec.push_back(0);

    char* utf8_str = u2utf8_copy(u_vec.data());

    hb_buffer_t* hb_buffer = hb_buffer_create();
    hb_buffer_add_utf8(hb_buffer, utf8_str, -1, 0, -1);

    // Set script and language
    hb_script_t hb_script = hb_script_from_iso15924_tag((uint32_t)script);
    hb_buffer_set_script(hb_buffer, hb_script);
    hb_language_t hb_lang = hb_language_from_string((const char*)lang, -1);
    hb_buffer_set_language(hb_buffer, hb_lang);

    bool rtl = !u_vec.empty() && isrighttoleft(u_vec[0]);
    hb_buffer_set_direction(hb_buffer,
                            rtl ? HB_DIRECTION_RTL : HB_DIRECTION_LTR);

    // Shape the text
    hb_shape(hb_ttf_font, hb_buffer, NULL, 0);

    // Retrieve the results
    SplineChar** glyphs_after_gpos = extract_shaped_data(hb_buffer);
    int glyph_count = metrics.size() - 1;

    std::vector<int> width_deltas =
        compute_width_deltas(hb_buffer, glyphs_after_gpos);

    if (rtl) {
        // HarfBuzz reverses the order of an RTL output buffer
        std::reverse(glyphs_after_gpos, glyphs_after_gpos + glyph_count);
        std::reverse(width_deltas.begin(), width_deltas.end());
    }

    // Zero-terminated list of features
    std::vector<uint32_t> flist(feature_list.begin(), feature_list.end());
    flist.push_back(0);

    // Apply legacy shaper for GPOS to retrieve kerning pair references. Metrics
    // calculated by the legacy shaper are ignored, except for kerning deltas.
    struct opentype_str* ots_arr = context_->apply_ticked_features(
        context_->sf, flist.data(), (uint32_t)script, (uint32_t)lang, true,
        pixelsize, glyphs_after_gpos);

    std::vector<int> kerning_deltas =
        compute_kerning_deltas(hb_buffer, ots_arr);

    // Perhaps counterintuitively, when setting RTL direction for RTL
    // languages, HarfBuzz would reverse the glyph order in the output
    // buffer. We therefore need to recompute metrics in reverse direction
    if (rtl) {
        metrics = reverse_rtl_metrics(metrics);
    }

    // Compute the accumulated shifts dx for each glyph as partial sums of
    // kerning and width deltas. Adjust glyph widths as appropriate.
    int shift = 0;
    for (int i = 0; i < glyph_count; ++i) {
        if (i > 0) {
            shift += (kerning_deltas[i - 1] + width_deltas[i - 1]);
        }
        metrics[i].dx += shift;
        metrics[i].dwidth += width_deltas[i];
    }

    // Cleanup
    hb_buffer_destroy(hb_buffer);
    free(utf8_str);

    return ots_arr;
}

void HarfBuzzShaper::scale_metrics(MetricsView* mv, double iscale, double scale,
                                   bool vertical) {
    int x0 = 10, y0 = 10;
    for (auto& m : metrics) {
        assert(!m.scaled);
        m.dx = x0 + m.dx * scale;
        m.dy = y0 + m.dy * scale;
        m.dwidth *= scale;
        m.dheight *= scale;
        m.xoff *= scale;
        m.yoff *= scale;
        m.scaled = true;
    }
}

}  // namespace ff::shapers
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
#pragma once

#include <map>
#include <memory>
#include <hb.h>

#include "i_shaper.hpp"
#include "shaper_shim.hpp"

namespace ff::shapers {

class HarfBuzzShaper : public IShaper {
 public:
    HarfBuzzShaper(std::shared_ptr<ShaperContext> context);
    ~HarfBuzzShaper();

    const char* name() const override { return "harfbuzz"; }

    struct opentype_str* apply_features(SplineChar** glyphs,
                                        const std::vector<Tag>& feature_list,
                                        Tag script, Tag lang,
                                        int pixelsize) override;

    void scale_metrics(MetricsView* mv, double iscale, double scale,
                       bool vertical) override;

 private:
    std::shared_ptr<ShaperContext> context_;

    char* blob = nullptr;
    hb_blob_t* hb_ttf_blob = nullptr;
    hb_face_t* hb_ttf_face = nullptr;
    hb_font_t* hb_ttf_font = nullptr;

    // Initial kerning state at font generation. For a pair of (left_glyph,
    // right_glyph) the shaper manually applies the difference between initial
    // and latest value to avoid regenerating the font at each change.
    //
    // NOTE: For completeness, this cache should have been keeping initial
    // kerning for each combination of features, but this would lead to
    // exponential storage, so we limit ourselves to the current combination, in
    // hope that it would be the same for all the other feature combinations.
    std::map<std::pair<hb_codepoint_t, hb_codepoint_t>, hb_position_t>
        initial_kerning_;

    // Initial width at font generation. The difference is applied similarly to
    // kerning deltas.
    //
    // NOTE: There is no need to keep glyph bearings, since HarfBuzz is not
    // responsible for glyph drawing, and the relative position of glyphs is
    // only affected by widths.
    std::map<hb_codepoint_t, hb_position_t> initial_width_;

    // Retrieve data from shaped buffer and fill metrics.
    SplineChar** extract_shaped_data(hb_buffer_t* hb_buffer);

    // RTL HarfBuzz shaping returns metrics end-to-start. This method reverses
    // them.
    std::vector<ShapeMetrics> reverse_rtl_metrics(
        const std::vector<ShapeMetrics>& reverse_metrics) const;

    // Compute changes in kerning due to user's input after the font was
    // generated.
    std::vector<int> compute_kerning_deltas(hb_buffer_t* hb_buffer,
                                            struct opentype_str* ots_arr);

    // Compute changes in glyph width due to user's input after the font was
    // generated.
    std::vector<int> compute_width_deltas(hb_buffer_t* hb_buffer,
                                          SplineChar** glyphs);
};

}  // namespace ff::shapers
/* Copyright 2026 Maxim Iorsh <iorsh@users.sourceforge.net>
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

#include "histogram.hpp"

#include <algorithm>

namespace ff::widgets {

Histogram::Histogram() {
    set_hexpand(true);
    set_vexpand(true);
    set_size_request(240, 160);
}

void Histogram::set_values(const std::vector<int>& values) {
    values_ = values;
    queue_draw();
}

bool Histogram::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();

    cr->set_source_rgb(1.0, 1.0, 1.0);
    cr->paint();

    if (values_.empty() || width <= 0 || height <= 0) {
        return true;
    }

    const int max_value = *std::max_element(values_.cbegin(), values_.cend());
    if (max_value <= 0) {
        return true;
    }

    const double bar_width = static_cast<double>(width) / values_.size();

    cr->set_source_rgb(0.125, 0.125, 1.0);
    for (size_t i = 0; i < values_.size(); ++i) {
        const double norm = static_cast<double>(values_[i]) / max_value;
        const double bar_height = norm * height;
        const double x = i * bar_width;
        const double y = height - bar_height;

        cr->rectangle(x, y, std::max(1.0, bar_width - 1.0), bar_height);
        cr->fill();
    }

    return true;
}

}  // namespace ff::widgets

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
#pragma once

#ifdef __cplusplus
#include <utility>
#include <vector>
#include "i_printer.hpp"
#endif

typedef struct PageState {
    int pt;
    int pointsize;
    int extrahspace;
    int ypos;
    int max;
    int page;
    int pageheight;
    int printtype;
    int lastfont;
    int wasps;
} PageState;

typedef struct PdfObjects {
// TODO(iorsh): remove hack due to use in C code in displayfonts.c
#ifdef __cplusplus
    std::vector<int>* offsets;
#else
    void* offsets;
#endif
    int* pages;
    int next_page;
    int max_page;
    long start_cur_page;
} PdfObjects;

#ifdef __cplusplus
struct printinfo;

namespace ff::layout {

enum class PrintPageStyle {
    regular,
    sample,
};

struct PrinterContext {
    ::printinfo* pi;
    void (*start_document_cb)(::printinfo*);
    void (*end_document_cb)(::printinfo*);
    void (*add_regular_page_cb)(::printinfo*);
    void (*add_sample_page_cb)(::printinfo*);
};

class LegacyPrinter final : public IPrinter {
 public:
    LegacyPrinter(PrintPageStyle page_style, PrinterContext context)
        : page_style_(page_style), context_(std::move(context)) {}

    void start_document() override { context_.start_document_cb(context_.pi); }

    void end_document() override { context_.end_document_cb(context_.pi); }

    void add_page() override {
        if (page_style_ == PrintPageStyle::sample)
            context_.add_sample_page_cb(context_.pi);
        else
            context_.add_regular_page_cb(context_.pi);
    }

 private:
    PrintPageStyle page_style_;
    PrinterContext context_;
};

}  // namespace ff::layout

#endif

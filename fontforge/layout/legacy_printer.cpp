/* Copyright (C) 2000-2012 by George Williams */
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

#include "legacy_printer.hpp"

#include <stdlib.h>

int pdf_addobject(PdfObjects& objects, FILE* out) {
    if (!objects.offsets)
        objects.offsets = new std::vector<int>({0}); /* Object 0 is magic */

    objects.offsets->push_back(ftell(out));
    fprintf(out, "%lu 0 obj\n", objects.offsets->size() - 1);
    return (objects.offsets->size() - 1);
}

void pdf_addpage(PdfObjects& objects, FILE* out) {
    if (!objects.pages) objects.pages = new std::vector<int>();

    objects.pages->push_back(objects.offsets->size());
    ::pdf_addobject(objects, out);
    fprintf(out, "<<\n");
    fprintf(out, "  /Parent 00000 0 R\n");
    fprintf(out, "  /Type /Page\n");
    fprintf(out, "  /Contents %lu 0 R\n", objects.offsets->size());
    fprintf(out, ">>\n");
    fprintf(out, "endobj\n");

    ::pdf_addobject(objects, out);
    fprintf(out, "<< /Length %lu 0 R >>\n", objects.offsets->size());
    fprintf(out, "stream\n");
    objects.start_cur_page = ftell(out);
}

void pdf_finishpage(PdfObjects& objects, FILE* out) {
    long streamlength;

    streamlength = ftell(out) - objects.start_cur_page;
    fprintf(out, "\nendstream\n");
    fprintf(out, "endobj\n");

    ::pdf_addobject(objects, out);
    fprintf(out, " %ld\n", streamlength);
    fprintf(out, "endobj\n\n");
}

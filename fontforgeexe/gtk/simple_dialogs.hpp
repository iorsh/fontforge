/* Copyright 2023 Joey Sabey <github.com/Omnikron13>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED “AS IS” AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "gresource.h"

typedef struct gwindow* GWindow;

enum ProblemRecType { prob_bool, prob_int, prob_double };

typedef struct {
    short cid;
    const char* label;
    const char* tooltip;
    bool active;
    enum ProblemRecType type;
    union {
        int ival;
        double dval;
    } value;
} ProblemRec;
#define PROBLEM_REC_EMPTY                      \
    {                                          \
        0, NULL, NULL, false, prob_bool, { 0 } \
    }

typedef struct {
    const char* label;
    ProblemRec* records;
} ProblemTab;
#define PROBLEM_TAB_EMPTY \
    { NULL, NULL }

int add_encoding_slots_dialog(bool cid);

/* This function updates pr_tabs in-place to preserve the state of the dialog
   between invocations.

   Return value: true, if any problem record was selected. The selected records
                 are marked as active in pr_tabs. */
bool find_problems_dialog(GWindow parent, ProblemTab* pr_tabs);

void update_appearance();

#ifdef __cplusplus
}
#endif

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
#include <cstdio>

int pdf_addobject(PdfObjects& objects, FILE *out) {
    if ( objects.next==0 ) {
	objects.max = 100;
	objects.offsets = (int *)malloc(objects.max*sizeof(int));
	objects.offsets[objects.next++] = 0;	/* Object 0 is magic */
    } else if ( objects.next>=objects.max ) {
	objects.max += 100;
	objects.offsets = (int *)realloc(objects.offsets,objects.max*sizeof(int));
    }
    objects.offsets[objects.next] = ftell(out);
    fprintf( out, "%d 0 obj\n", objects.next++ );
return( objects.next-1 );
}


/* iff.h
 *
 * This file is part of fizmo.
 *
 * Copyright (c) 2009-2011 Christoph Ender.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef iff_h_INCLUDED 
#define iff_h_INCLUDED

#include <stdio.h>

#include "../tools/types.h"

#define IFF_MODE_READ 0
#define IFF_MODE_WRITE 1

/*@null@*/ /*@dependent@*/ void *open_simple_iff_file(char *filename, int mode);
int start_new_chunk(char *id, void *iff_file);
int end_current_chunk(void *iff_file);
int close_simple_iff_file(void *iff_file);
int find_chunk(char *id, void *iff_file);
int read_chunk_length(void *iff_file);
int get_last_chunk_length();
int write_four_byte_number(uint32_t number, void *iff_file);
uint32_t read_four_byte_number(void *iff_file);
char *read_form_type(void *iff_file);
bool is_form_type(void *iff_file, char* form_type);

#endif /* iff_h_INCLUDED */


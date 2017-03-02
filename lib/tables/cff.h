/*

  Fluxfonts – a continual font generator for increased privacy
  Copyright 2012–2017, Daniel Aleksandersen
  Copyright 2012, Daniel Nebdal
  All rights reserved.

  This file is part of Fluxfonts.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  • Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

  • Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
  COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef __cff_include

#if defined( _WIN32 ) || defined( _WIN64 )
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

#include "../buffer.h"
#include "../familyname.h"

typedef struct cff_header {
  uint8_t majorVersion;
  uint8_t minorVersion;
  uint8_t headerSize;
  uint8_t offsetSize;
} __attribute__( ( packed ) ) CFF_HEADER;

typedef struct cff_index_empty {
  uint16_t count;
} __attribute__( ( packed ) ) CFF_INDEX_EMPTY;

typedef struct cff_index_single_entry {
  uint16_t count;
  uint8_t offsetSize;
  uint8_t offset;
  uint8_t length;
} __attribute__( ( packed ) ) CFF_INDEX_SINGLE_ENTRY;

typedef struct cff_index_string {
  uint16_t count;
  uint8_t offsetSize;
  uint8_t offset1;
  uint8_t offset2;
  uint8_t offset3;
  uint8_t length;
} __attribute__( ( packed ) ) CFF_INDEX_STRING;

typedef struct cff_index_chars {
  uint16_t count;
  uint8_t offsetSize;
  uint8_t offset1;
  uint8_t offset2;
  uint8_t offset3;
  uint8_t offset4;
  uint8_t length;
} __attribute__( ( packed ) ) CFF_INDEX_CHARS;

typedef struct private_dict_offsets {
  int charstrings_pos;
  int private_pos;
  int encoding_pos;
  int charset_pos;
  int private_len;
} PRIVATE_DICT_OFFSETS;

struct cff_result {
  BUFFER *cffData;
  CFF_INDEX_CHARS *indexChars;
};

int get_encoded_charstring_len( int number );
BUFFER *encode_charstring_number( int number );
int decode_charstring_number( BUFFER *number );
struct cff_result *set_table_cff( struct names *font_names );
BUFFER *make_topdict_offsets( PRIVATE_DICT_OFFSETS *offsets );

#define __cff_include
#endif

/*

  Fluxfonts – a continual font generator for increased privacy
  Copyright 2012–2017, Daniel Aleksandersen
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

#ifndef __tables_included

#if defined( _WIN32 ) || defined( _WIN64 )
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

#include "../buffer.h"

typedef struct st_otf_offset_table {
  union {
    struct {
      uint16_t major;
      uint16_t minor;
    } version;
    char tag[4];
  } versiontag;
  uint16_t numTables;
  uint16_t searchRange;
  uint16_t entrySelector;
  uint16_t rangeShift;
} __attribute__( ( packed ) ) OTF_OFFSET_TABLE;

typedef struct st_otf_table_record {
  char tag[4];
  uint32_t checkSum;
  uint32_t offset;
  uint32_t length;
} __attribute__( ( packed ) ) OTF_TABLE_RECORD;

void *alloc_table_record_entry( BUFFER *buf, char *tag );
uint32_t calc_table_checksum( void *start, size_t length );
void set_table_records( BUFFER *fontbuffer, OTF_TABLE_RECORD *table );

#define __tables_included
#endif

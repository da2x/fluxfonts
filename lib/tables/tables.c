/*

  Fluxfonts – a continual font generator for increased privacy
  Copyright 2012–2017, Daniel Aleksandersen
  All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-FreeBSD
  License-Filename: COPYING

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

#include <stdint.h>

#if defined( _WIN32 ) || defined( _WIN64 )
typedef uint16_t wchar_t;
#endif

#include "tables.h"

void *alloc_table_record_entry( BUFFER *buf, char *tag ) {

  OTF_TABLE_RECORD *result = buffer_alloc( buf, sizeof( OTF_TABLE_RECORD ) );
  memcpy( result->tag, tag, 4 );

  return result;
}

uint32_t calc_table_checksum( void *start, size_t length ) {

  uint32_t *pos = (uint32_t *) start;
  uint32_t *end = (uint32_t *) ( (size_t) pos + length );
  uint32_t checksum = 0;

  while ( pos < end ) {
    checksum += htonl( *pos );
    pos++;
  }

  return checksum;
}

void set_table_records( BUFFER *fontbuffer, OTF_TABLE_RECORD *table ) {

  uint32_t *pos = (uint32_t *) &fontbuffer->data[table->offset];
  uint32_t checksum = calc_table_checksum( pos, table->length );
  table->offset = htonl( table->offset );
  table->length = htonl( table->length );
  table->checkSum = htonl( checksum );
}

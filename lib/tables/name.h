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

#ifndef __name_include

#if defined( _WIN32 ) || defined( _WIN64 )
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

#include "../buffer.h"
#include "../familyname.h"

typedef struct st_otf_table_name {
  uint16_t format;
  uint16_t count;
  uint16_t stringOffset;
} __attribute__( ( packed ) ) OTF_TABLE_NAME;

typedef struct st_name_record {
  uint16_t platformID;
  uint16_t encodingID;
  uint16_t languageID;
  uint16_t nameID;
  uint16_t length;
  uint16_t offset;
} __attribute__( ( packed ) ) NAME_RECORD;

void put_namerecord( BUFFER *buf, BUFFER *stringbuf, char *name,
                     uint16_t platformID, uint16_t encodingID,
                     uint16_t languageID, uint16_t nameID );
void put_namerecord_w( BUFFER *buf, BUFFER *stringbuf, char *name,
                       uint16_t platformID, uint16_t encodingID,
                       uint16_t languageID, uint16_t nameID );

BUFFER *set_table_name( struct names *font_names );

#define __name_include
#endif

/*

  Fluxfonts – a continual font generator for increased privacy
  Copyright 2012–2016, Daniel Aleksandersen
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


#ifndef __cmap_include

#include <arpa/inet.h>

#include "../buffer.h"


typedef struct st_otf_cmap_encoding_record {
  uint16_t  platformID;
  uint16_t  encodingID;
  uint32_t  offset;
} __attribute__((packed)) OTF_CMAP_ENCODING_RECORD;


typedef struct st_otf_table_cmap {
  uint16_t  version;
  uint16_t  numTables;
  OTF_CMAP_ENCODING_RECORD record1;
  OTF_CMAP_ENCODING_RECORD record2;
  OTF_CMAP_ENCODING_RECORD record3;
}  __attribute__((packed)) OTF_TABLE_CMAP;


typedef struct st_otf_cmap_format4_enctable {
  uint16_t  format;
  uint16_t  length;
  uint16_t  language;
  uint16_t  segCountX2;
  uint16_t  searchRange;
  uint16_t  entrySelector;
  uint16_t  rangeShift;
  uint16_t  endCount1;
  uint16_t  endCount2;
  uint16_t  endCount3;
  uint16_t  reservedPad;
  uint16_t  startCount1;
  uint16_t  startCount2;
  uint16_t  startCount3;
   int16_t  idDelta1;
   int16_t  idDelta2;
   int16_t  idDelta3;
  uint16_t  idRangeOffset1;
  uint16_t  idRangeOffset2;
  uint16_t  idRangeOffset3;
} __attribute__((packed)) OTF_CMAP_FORMAT4_ENCTABLE;


typedef struct st_otf_cmap_format6_enctable {
  uint16_t  format;
  uint16_t  length;
  uint16_t  language;
  uint16_t  firstCode;
  uint16_t  entryCount;
  uint16_t  entry[38];
} __attribute__((packed)) OTF_CMAP_FORMAT6_ENCTABLE;


BUFFER *set_table_cmap( void );


#define __cmapp_include
#endif

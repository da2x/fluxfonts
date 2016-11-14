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


#ifndef __hhea_include

#include <arpa/inet.h>

#include "../buffer.h"


typedef struct st_otf_table_hhea {
  struct {
    uint16_t major;
    uint16_t minor;
  } version;
   int16_t  ascender;
   int16_t  descender;
   int16_t  lineGap;
  uint16_t  advanceWidthMax;
   int16_t  minLeftSideBearing;
   int16_t  minRightSideBearing;
   int16_t  xMaxExtent;
   int16_t  caretSlopeRise;
   int16_t  caretSlopeRun;
   int16_t  caretOffset;
   int16_t  reserved0;
   int16_t  reserved1;
   int16_t  reserved2;
   int16_t  reserved3;
   int16_t  metricDataFormat;
  uint16_t  numberOfHMetrics;
}  __attribute__((packed)) OTF_TABLE_HHEA;


BUFFER *set_table_hhea( );


#define __hhea_include
#endif

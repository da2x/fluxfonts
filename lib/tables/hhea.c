/*

  Fluxfonts – a continual font generator for increased privacy
  Copyright 2012–2013, Daniel Aleksandersen
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


#include "hhea.h"


BUFFER *set_table_hhea( void ) {

  BUFFER *hhea_buf = makebuffer( sizeof( OTF_TABLE_HHEA ) );
  OTF_TABLE_HHEA *t_hheatable = buffer_alloc( hhea_buf, sizeof( OTF_TABLE_HHEA ) );
  t_hheatable->version.major = htons( 1 );
  t_hheatable->ascender = htons( 732 );
  t_hheatable->descender = htons( -268 );
  t_hheatable->lineGap = htons( 65036 );
  t_hheatable->advanceWidthMax = htons( 500 );
  t_hheatable->xMaxExtent = htons( 500 );
  t_hheatable->caretSlopeRise = htons( 1 );
  t_hheatable->lineGap = htons( 1 );
  t_hheatable->numberOfHMetrics = htons( 4 );

  return hhea_buf;
}

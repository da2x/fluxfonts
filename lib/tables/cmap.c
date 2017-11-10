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

#include "cmap.h"

BUFFER *set_table_cmap( void ) {

  BUFFER *cmap_buf = makebuffer( sizeof( OTF_TABLE_CMAP ) +
                                 sizeof( OTF_CMAP_FORMAT4_ENCTABLE ) +
                                 sizeof( OTF_CMAP_FORMAT6_ENCTABLE ) );
  OTF_TABLE_CMAP *t_cmaptable =
      buffer_alloc( cmap_buf, sizeof( OTF_TABLE_CMAP ) );
  t_cmaptable->version = 0;
  t_cmaptable->numTables = htons( 3 );
  t_cmaptable->record1.platformID = 0;
  t_cmaptable->record1.encodingID = htons( 3 );
  t_cmaptable->record2.platformID = htons( 1 );
  t_cmaptable->record2.encodingID = 0;
  t_cmaptable->record3.platformID = htons( 3 );
  t_cmaptable->record3.encodingID = htons( 1 );

  OTF_CMAP_FORMAT4_ENCTABLE *t_cmap_encode4 =
      buffer_alloc( cmap_buf, sizeof( OTF_CMAP_FORMAT4_ENCTABLE ) );
  t_cmap_encode4->format = htons( 4 );
  t_cmap_encode4->length = htons( 40 );
  t_cmap_encode4->segCountX2 = htons( 6 );
  t_cmap_encode4->searchRange = htons( 4 );
  t_cmap_encode4->entrySelector = htons( 1 );
  t_cmap_encode4->rangeShift = htons( 2 );
  t_cmap_encode4->endCount1 = htons( 32 );
  t_cmap_encode4->endCount2 = htons( 46 );
  t_cmap_encode4->endCount3 = htons( 65535 );
  t_cmap_encode4->startCount1 = htons( 32 );
  t_cmap_encode4->startCount2 = htons( 46 );
  t_cmap_encode4->startCount3 = htons( 65535 );
  t_cmap_encode4->idDelta1 = htons( -31 );
  t_cmap_encode4->idDelta2 = htons( -44 );
  t_cmap_encode4->idDelta3 = htons( 1 );

  OTF_CMAP_FORMAT6_ENCTABLE *t_cmap_encode6 =
      buffer_alloc( cmap_buf, sizeof( OTF_CMAP_FORMAT6_ENCTABLE ) );
  t_cmap_encode6->format = htons( 6 );
  t_cmap_encode6->length = htons( 86 );
  t_cmap_encode6->firstCode = htons( 9 );
  t_cmap_encode6->entryCount = htons( 38 );
  t_cmap_encode6->entry[0] = htons( 1 );
  t_cmap_encode6->entry[4] = htons( 1 );
  t_cmap_encode6->entry[23] = htons( 1 );
  t_cmap_encode6->entry[37] = htons( 2 );

  /* Record one and three point at the same character. Record two is offset
   * immediatly following record one. */
  t_cmaptable->record1.offset = htonl( sizeof( OTF_TABLE_CMAP ) );
  t_cmaptable->record2.offset =
      htonl( sizeof( OTF_TABLE_CMAP ) + sizeof( OTF_CMAP_FORMAT4_ENCTABLE ) );
  t_cmaptable->record3.offset = htonl( sizeof( OTF_TABLE_CMAP ) );

  return cmap_buf;
}

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


#include "head.h"


BUFFER *set_table_head( void ) {

  BUFFER *head_buf = makebuffer( sizeof( OTF_TABLE_HEAD ) );
  OTF_TABLE_HEAD *t_headtable = buffer_alloc( head_buf, sizeof( OTF_TABLE_HEAD ) );
  t_headtable->majorVersion = htons( 1 );
  t_headtable->minorVersion = 0;
  t_headtable->majorRevision = htons( 1 );
  t_headtable->minorRevision = 0;
  t_headtable->magicNumber = htonl( 0x5F0F3CF5 );
  t_headtable->flags = 0;
  t_headtable->unitsPerEm = htons( 1600 );
  t_headtable->created_b =  htonl( 0xCC34D839 );
  t_headtable->created_a = 0;
  t_headtable->modified_b = htonl( 0xCC34D839 );
  t_headtable->modified_a = 0;
  t_headtable->xMin = 0;
  t_headtable->yMin = htons( -300 );
  t_headtable->xMax = htons( 16 );
  t_headtable->yMax = htons( 800 );
  t_headtable->macStyle = 0;
  t_headtable->lowestRecPPEM = htons( 16 );
  t_headtable->fontDirectionHint = htons( 2 );
  t_headtable->indexToLocFormat = 0;
  t_headtable->glyphDataFormat = 0;

  return head_buf;
}

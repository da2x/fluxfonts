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


#include "post.h"


BUFFER *set_table_post( void ) {

  BUFFER *post_buf = makebuffer( sizeof( OTF_TABLE_POST ) );
  OTF_TABLE_POST *t_posttable = buffer_alloc( post_buf, sizeof( OTF_TABLE_POST ) );
  t_posttable->version.major = htons( 3 );
  t_posttable->italicAngle = htonl( 0 );
  t_posttable->underlinePosition = htons( -75 );
  t_posttable->underlineThickness = htons( 5 );
  t_posttable->isFixedPitch = htonl( 1 );
  t_posttable->minMemType42 = htonl( 0 );
  t_posttable->maxMemType42 = htonl( 0 );
  t_posttable->minMemType1 = htonl( 0 );
  t_posttable->maxMemType1 = htonl( 0 );

  return post_buf;
}

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

#ifndef __opentype_include

#if defined( _WIN32 ) || defined( _WIN64 )
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

#include "wincompatstdint.h"

#ifdef _WIN32
#define _X86_
#endif
#ifdef _WIN64
#define _AMD64_
#endif

#if defined( _WIN32 ) || defined( _WIN64 )
#include <Winreg.h>
#include <io.h>
#define access _access
#define close _close
#define write _write
#endif

#include <math.h>

#include "buffer.h"
#include "familyname.h"

#include "tables/cff.h"
#include "tables/cmap.h"
#include "tables/head.h"
#include "tables/hhea.h"
#include "tables/hmtx.h"
#include "tables/maxp.h"
#include "tables/name.h"
#include "tables/os2.h"
#include "tables/post.h"
#include "tables/tables.h"

BUFFER *assemble_opentype_font( struct names *font_names );
void *insert_full_table( BUFFER *complete_font, OTF_TABLE_RECORD *head_entry,
                         BUFFER *insert_me );
int font_generator( void );

extern char *datadir;
extern char *fontsetlist;
extern char *fontdir;

#define __opentype_include
#endif
/*

  Fluxfonts – a continual font generator for increased privacy
  Copyright 2012–2017, Daniel Aleksandersen
  Copyright 2012, Daniel Nebdal
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

#ifndef __names_include

#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#if defined( _WIN32 ) || defined( _WIN64 )
#define rand util_winrand
#else
#include <sys/mman.h>
#include <unistd.h>
#endif

#include "buffer.h"
#include "utils.h"

struct names {
  const char *copyrightNotice;
  char *fontFamilyName;
  const char *fontFamilyStyle;
  char *uniqueFontName;
  char *versionString;
  char *fullFontName;
  char *postscriptName;
  char *foundryVendorID;
};

struct names *make_fontnames( );
char *random_majuscule_characters( int numberOfCharacters );
char *random_minuscule_characters( int numberOfCharacters );
char *random_word( );
BUFFER *mmap_file( char *filename );
POSITIONS *count_lines( BUFFER *data );
char *word_from_pos( char *data );

extern BUFFER *dict_file;
extern POSITIONS *dict_lines;

#define __names_include
#endif

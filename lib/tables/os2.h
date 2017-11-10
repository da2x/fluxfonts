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

#ifndef __os2_include

#if defined( _WIN32 ) || defined( _WIN64 )
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

#include "../buffer.h"
#include "../familyname.h"

typedef struct st_otf_table_os2 {
  uint16_t version;
  int16_t xAvgCharWidth;
  uint16_t usWeightClass;
  uint16_t usWidthClass;
  uint16_t fsType;
  int16_t ySubscriptXSize;
  int16_t ySubscriptYSize;
  int16_t ySubscriptXOffset;
  int16_t ySubscriptYOffset;
  int16_t ySuperscriptXSize;
  int16_t ySuperscriptYSize;
  int16_t ySuperscriptXOffset;
  int16_t ySuperscriptYOffset;
  int16_t yStrikeoutSize;
  int16_t yStrikeoutPosition;
  int16_t sFamilyClass;
  char panose[10];
  uint32_t ulUnicodeRange1;
  uint32_t ulUnicodeRange2;
  uint32_t ulUnicodeRange3;
  uint32_t ulUnicodeRange4;
  char achVendID[4];
  uint16_t fsSelection;
  uint16_t usFirstCharIndex;
  uint16_t usLastCharIndex;
  int16_t sTypoAscender;
  int16_t sTypoDescender;
  int16_t sTypoLineGap;
  uint16_t usWinAscent;
  uint16_t usWinDescent;
  uint32_t ulCodePageRange1;
  uint32_t ulCodePageRange2;
  int16_t sxHeight;
  int16_t sCapHeight;
  uint16_t usDefaultChar;
  uint16_t usBreakChar;
  uint16_t usMaxContent;
} __attribute__( ( packed ) ) OTF_TABLE_OS2;

BUFFER *set_table_os2( struct names *font_names );

#define __os2_include
#endif

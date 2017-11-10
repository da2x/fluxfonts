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

#include "os2.h"

BUFFER *set_table_os2( struct names *font_names ) {

  BUFFER *os2_buf = makebuffer( sizeof( OTF_TABLE_OS2 ) );
  OTF_TABLE_OS2 *t_os2table = buffer_alloc( os2_buf, sizeof( OTF_TABLE_OS2 ) );
  t_os2table->version = htons( 3 );
  t_os2table->xAvgCharWidth = htons( 300 );
  t_os2table->usWeightClass = htons( 400 );
  t_os2table->usWidthClass = htons( 5 );
  t_os2table->fsType = htons( 0 );
  t_os2table->ySubscriptXSize = htons( 400 );
  t_os2table->ySubscriptYSize = htons( 400 );
  t_os2table->ySubscriptXOffset = htons( 50 );
  t_os2table->ySubscriptYOffset = htons( 50 );
  t_os2table->ySuperscriptXSize = htons( 400 );
  t_os2table->ySuperscriptYSize = htons( 400 );
  t_os2table->ySuperscriptXOffset = htons( 50 );
  t_os2table->ySuperscriptYOffset = htons( 50 );
  t_os2table->yStrikeoutSize = htons( 1 );
  t_os2table->yStrikeoutPosition = htons( 8 );
  t_os2table->sFamilyClass = htons( 0 );
  t_os2table->panose[0] = 2; // Latin text.
  t_os2table->panose[1] = 2; // Cove style
  t_os2table->panose[2] = 5; // Book weight
  t_os2table->panose[3] = 9; // Monospace
  t_os2table->panose[4] = 8; // High contrast
  t_os2table->panose[5] = 0; // Any stroke variation
  t_os2table->panose[6] = 0; // Any arm
  t_os2table->panose[7] = 9; // Oblique letterform
  t_os2table->panose[8] = 0; // Any midline
  t_os2table->panose[9] = 3; // Constant x-height
  t_os2table->ulUnicodeRange1 = htonl( 1 );
  t_os2table->ulUnicodeRange2 = htonl( 0 );
  t_os2table->ulUnicodeRange3 = htonl( 0 );
  t_os2table->ulUnicodeRange4 = htonl( 0 );
  memcpy( t_os2table->achVendID, font_names->foundryVendorID, 4 );
  t_os2table->fsSelection = htons( 0 );
  t_os2table->usFirstCharIndex = htons( 32 );
  t_os2table->usLastCharIndex = htons( 0x2e );
  t_os2table->sTypoAscender = htons( 732 );
  t_os2table->sTypoDescender = htons( -300 );
  t_os2table->sTypoLineGap = htons( 68 );
  t_os2table->usWinAscent = htons( 732 );
  t_os2table->usWinDescent = htons( 268 );
  t_os2table->ulCodePageRange1 = htonl( 1 );
  t_os2table->ulCodePageRange2 = htonl( 0 );
  t_os2table->sxHeight = htons( 8 );
  t_os2table->sCapHeight = htons( 16 );
  t_os2table->usDefaultChar = htons( 0 );
  t_os2table->usBreakChar = htons( 32 );
  t_os2table->usMaxContent = htons( 0 );

  return os2_buf;
}

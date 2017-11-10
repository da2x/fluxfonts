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

#include "wincompatstdint.h"

#ifdef _WIN32
#define _X86_
#endif
#ifdef _WIN64
#define _AMD64_
#endif

#include <errno.h>

#if defined( _WIN32 ) || defined( _WIN64 )
#define _CRT_RAND_S
#endif

#include <stdlib.h>

#include <stdio.h>
#include <string.h>

#if defined( _WIN32 ) || defined( _WIN64 )
#include <FileAPI.h>
#include <Windef.h>
#include <WinBase.h>
#include <Winreg.h>
#include <direct.h>
#include <io.h>

#define access _access
#define chmod _chmod
#define mkdir _mkdir
#define unlink _unlink
#define rand util_winrand
#else
#include <glob.h>
#include <sys/file.h>
#include <syslog.h>
#include <unistd.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>

#include "define.h"

void util_single_process_lock( void );
void util_daemonize_self( void );
void util_init_rand( void );

int util_power_supply_online( void );
int util_maxpow2( int val );
char *util_get_datadir( void );
char *util_get_fontdir( void );
void util_uninstall_all_fonts( char *fontsetlist, char *fontdir );
int util_really_remove_file( char *path );
uint8_t *util_charptr_to_usc2be( char *str );

#if defined( _WIN32 ) || defined( _WIN64 )
int util_winrand( void );
#endif
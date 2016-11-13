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


#include "main.h"


char *datadir = NULL;
char *fontsetlist = NULL;
char *fontdir = NULL;


int main() {

  util_daemonize_self( );
  util_single_process_lock( );

  if ( datadir == NULL )
    datadir = util_get_datadir();
  if ( fontsetlist == NULL ) {
    fontsetlist = ( char* ) calloc( ( strlen( datadir ) + strlen( INSTALLED_FONTS_FILE ) + 1 ), sizeof( char ) );
    strcat( fontsetlist, datadir );
    strcat( fontsetlist, INSTALLED_FONTS_FILE );
  }
  if ( fontdir == NULL )
    fontdir = util_get_fontdir();

  util_init_rand();

/* Let systemd know the service is ready. */
#if ( defined SYSTEMD )
  sd_notify( 0, "READY=1" );
#endif

  /* Main loop that continuously makes and replaces new fonts. */
  for ( ;; ) {

    /* Check that required folders are still in place. Recreate or exit when needed. */
    if ( access( datadir, R_OK | X_OK | W_OK ) != 0 )
      util_get_datadir();
    if ( access( fontdir, R_OK | X_OK | W_OK ) != 0 )
      util_get_fontdir();

    /* Delete all previously installed fonts */
    util_uninstall_all_fonts( fontsetlist, fontdir );

    /* Assemble and install one–six new fonts */
    int num_new_fonts = ( rand() % 5 ) + 1;
    for ( int i=0; i < num_new_fonts; i++ ) {
      font_generator();
    }
    if ( dict_file != NULL ) {
      munmap( dict_file->data, dict_file->size );
      free( dict_file );
      dict_file = NULL;
    }

    /* When a power source is online, sleep for between 2 and 20 minutes
       When on battery power, sleep for between 15 and 40 minutes before the next run. */
    int sleep_cycle = 120;
    if ( util_power_supply_online( ) )
      sleep_cycle = 120 + ( rand() % 1080 );
    else
      sleep_cycle = 900 + ( rand() % 1500 );

    sleep( sleep_cycle );
  }

  return 0; /* When did 1 stop being true? */
}

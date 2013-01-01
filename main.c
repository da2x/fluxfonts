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


#include "main.h"


char *varlibdir = NULL;
char *fontsetlist = NULL;
char *fontdir = NULL;


int main() {

  util_daemonize_self( );
  util_single_process_lock( );

  if ( varlibdir == NULL )
    varlibdir = util_get_varlibdir();
  if ( fontsetlist == NULL ) {
    fontsetlist = ( char* ) calloc( ( strlen( varlibdir ) + strlen( INSTALLED_FONTS_FILE ) + 1 ), sizeof( char ) );
    strcat( fontsetlist, varlibdir );
    strcat( fontsetlist, INSTALLED_FONTS_FILE );
  }
  if ( fontdir == NULL )
    fontdir = util_get_fontdir();

  util_init_rand();

  /* Main loop that continuously makes and replaces new fonts. */
  for ( ;; ) {

    /* Check that required folders are still in place. Recreate or exit when needed. */
    if ( !util_test_path_access ( varlibdir ) )
      util_get_varlibdir();
    if ( !util_test_path_access ( fontdir ) )
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

    /* Post install */
    util_rebuild_fontcache( fontdir );

    /* End of loop.
     * Sleep for a random interval of 7:30 minutes to 35 minutes delaying the next run.
     */
    int sleep_cycle = ( rand() % 1650 ) + 450;
    sleep( sleep_cycle );
  }

  return 0; /* When did 1 stop being true? */
}

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


#include "utils.h"


void util_single_process_lock( void ) {

  char *lockfile = util_get_lockfile( );
  int lfd = -1;
  lfd = open( lockfile, O_RDONLY|O_CREAT|O_NONBLOCK, 0640 );
  if ( lfd < 0 ) {
    syslog( LOG_ERR, "Cannot open lock file %s (%s)", lockfile, strerror( errno ) );
    exit( errno );
  }
  if ( flock(lfd, LOCK_EX|LOCK_NB) != 0 ) {
      syslog( LOG_ERR, "Cannot get lock on file %s (%s)", lockfile, strerror( errno ) );
      exit( errno );
  }

  free ( lockfile ); /* path only */
}


void util_daemonize_self( void ) {

  pid_t pid, sid;

  /* already running as a daemon process */
  if ( getppid() == 1 )
    return;

  pid = fork();
  if ( pid < 0 )
    exit( EXIT_FAILURE );
  if ( pid > 0 )
    exit( 0 );

  umask( 0 );

  sid = setsid();
  if ( sid < 0 )
    exit( EXIT_FAILURE );
  if ( ( chdir( "/" ) ) < 0 )
    exit( EACCES );

  /* Redirect output to nowhere */
  freopen( "/dev/null", "r", stdin );
  freopen( "/dev/null", "w", stdout );
  freopen( "/dev/null", "w", stderr );
}


void util_init_rand( void ) {

  int random_file = open( "/dev/urandom", O_RDONLY );
  int random_number;
  read( random_file, &random_number, sizeof( random_number ) );
  close( random_file );
  srand( random_number );
}


char* util_get_lockfile( void ) {

  char* lockfile = ( char* ) malloc( 1 );
  *lockfile = '\0';

  if( util_test_path_access( "/run/" ) )
    lockfile = util_descend_required_dir( lockfile, "/run",  0755 );
  else /* older Linuses and OS X. */
    lockfile = util_descend_required_dir( lockfile, "/var",  0755 );
  lockfile = util_descend_required_dir( lockfile, "lock", 0777 );
  lockfile = realloc( lockfile, strlen( "/run/lock/" ) + strlen( PROGRAM_NAME ) + 2 );
  strcat( lockfile, PROGRAM_NAME );

  return lockfile;
}


char* util_get_varlibdir( void ) {

  char* varlibdir = ( char* ) malloc( 1 );
  *varlibdir = '\0';

  varlibdir = util_descend_required_dir( varlibdir, "/var",  0755 );
  varlibdir = util_descend_required_dir( varlibdir, "lib", 0755 );
  varlibdir = util_descend_required_dir( varlibdir, PROGRAM_NAME, 0775 );

  return varlibdir;
}


char* util_get_fontdir( void ) {

  /* Will use the known OS X directory for fonts, or else try known
   * font directories based on the Filesystem Hierarchy standard.
   */

  char* fontdir = ( char* ) malloc( 1 );
  *fontdir = '\0';

  if( util_test_path_access( "/Library/Fonts/" ) ) { /* assume it is OS X */
    fontdir = util_descend_required_dir( fontdir, "/Library/Fonts", 0775 );
  }

  else if( util_test_path_access( "/usr/local/share/fonts/" ) ) {
    fontdir = util_descend_required_dir( fontdir, "/usr",  0755 );
    fontdir = util_descend_required_dir( fontdir, "local", 0755 );
    fontdir = util_descend_required_dir( fontdir, "share", 0755 );
    fontdir = util_descend_required_dir( fontdir, "fonts", 0775 );
  }

  else { /* fallback. */
    fontdir = util_descend_required_dir( fontdir, "/usr",  0755 );
    fontdir = util_descend_required_dir( fontdir, "share", 0755 );
    fontdir = util_descend_required_dir( fontdir, "fonts", 0775 );
  }

  return fontdir;
}


int util_test_path_access( char* path ) { /* true with access */

  if ( access( path, R_OK | X_OK | W_OK ) == 0 )
    return 1;
  return 0;
}


void util_require_dir_access( char* path ) {

  /* Only used for absolutely required directories. Will exit with insufficient permissions. */
  if ( !util_test_path_access( path ) ) {
    syslog( LOG_ERR, "Directory at %s is not accessible. (%s)", path, strerror( errno ) );
    exit( EACCES );
  }
}


char* util_descend_required_dir( char* dirpath, const char* subdir, int permissions ) {

  char* returnpath = ( char* ) calloc( ( strlen( dirpath ) + strlen( subdir ) + 2 ), sizeof( char ) );
  strcat( returnpath, dirpath );
  strcat( returnpath, subdir );
  strcat( returnpath, "/" );
  free( dirpath );
  mkdir( returnpath, permissions );
  util_require_dir_access( returnpath );

  return returnpath;
}


void util_uninstall_all_fonts( char* fontsetlist, char* fontdir ) {

  if ( access( fontsetlist, F_OK | R_OK ) == 0 ) {
    FILE *fontsetlist_file = fopen( fontsetlist, "r" );
    char fontname[200];

    while( fgets( fontname, 200, fontsetlist_file ) != NULL ) {

      if ( strstr( fontname, "/") != NULL ) {
        syslog( LOG_WARNING, "The file '%s' has unexpected alterations or may have been corrupted. The string '%s' should never have contained a slash!", fontsetlist, fontname );
        continue;
      }

      char *pos;
      char *path = calloc( ( strlen( fontdir ) + strlen( fontname ) + strlen( FONTFILE_EXTENSION ) + 1 ), sizeof( char ) );

      if ( ( pos=strchr( fontname, '\n' ) ) != NULL )
        *pos = '\0';

      strcat( path, fontdir );
      strcat( path, fontname );
      strcat( path, FONTFILE_EXTENSION );

      unlink( path );
      free( path );
    }

    fclose( fontsetlist_file );
    unlink( fontsetlist );
  }
}


int util_maxpow2( int val ) {

  if ( val == 1 ) return 0;

  int result = 1;
  while ( result <= val ) result = result << 1;

  return result >> 1;
}


int util_rebuild_fontcache( char* dirpath ) {

#if ( ( defined __unix__ ) || ( defined __linux__ ) ) && !( defined __APPLE__ )
  /* Best (meaning very little) effort font cache rebuilding on Linux/FreeBSD. */
  if ( fork() == 0 ) {
    execlp( "fc-cache", dirpath );
    return errno;
  }
#endif
  return 0;
}

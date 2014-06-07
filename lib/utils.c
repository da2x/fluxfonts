/*

  Fluxfonts – a continual font generator for increased privacy
  Copyright 2012–2014, Daniel Aleksandersen
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

  char* lockfile = ( char* ) malloc( ( strlen( "/run/lock/" ) + strlen( PROGRAM_NAME ) + 1 ) );
  *lockfile = '\0';

  if ( access( "/run/lock/", R_OK | W_OK | X_OK ) == 0 )
    strcat( lockfile, "/run/lock/" );
  else if ( access( "/var/lock/", R_OK | W_OK | X_OK ) == 0 )
    strcat( lockfile, "/var/lock/" );
  else {
    syslog( LOG_CRIT, "Neither the `/run/lock/` nor `/var/lock/` directories are read-writeable!" );
    exit( EACCES );
  }

  strcat( lockfile, PROGRAM_NAME );

  int lfd = -1;
  lfd = open( lockfile, O_RDONLY | O_CREAT | O_NONBLOCK, 0640 );
  if ( lfd < 0 ) {
    syslog( LOG_ERR, "Cannot open lock file %s (%s)", lockfile, strerror( errno ) );
    exit( errno );
  }
  if ( flock( lfd, LOCK_EX | LOCK_NB ) != 0 ) {
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
  if ( freopen( "/dev/null", "r", stdin  ) == NULL )
    perror( "Failed to supress this output when redirecting stdin."  );
  if ( freopen( "/dev/null", "w", stdout ) == NULL )
    perror( "Failed to supress this output when redirecting stdout." );
  if ( freopen( "/dev/null", "w", stderr ) == NULL )
    perror( "Failed to supress this output when redirecting stderr." );
}


void util_init_rand( void ) {

  int random_number;
  int random_file = open( "/dev/urandom", O_RDONLY );

  if ( read( random_file, &random_number, sizeof( random_number) ) != sizeof( random_number ) )
    perror( "Random error. Literally. /dev/urandom did not produce any output." );

  close( random_file );
  srand( random_number );
}


char* util_get_datadir( void ) {

  char* libdir = "/var/lib/";

  char* datadir = ( char* ) malloc( ( strlen( libdir ) + strlen( PROGRAM_NAME ) + 2 ) );
  *datadir = '\0';

  strcpy( datadir, libdir );
  strcat( datadir, PROGRAM_NAME );
  strcat( datadir, "/" );

  if ( access( datadir, R_OK | W_OK | X_OK ) != 0 ) {
    if ( access( libdir, R_OK | W_OK | X_OK ) == 0 ) {
      /* optimistic problem correction */
      mkdir( datadir, 0750 );
      chmod( datadir, 0750 );
    }
    if ( access( datadir, R_OK | W_OK | X_OK ) != 0 ) {
      syslog( LOG_ERR, "The `%s` directory is not read-writeable or could not be created.", datadir );
      exit( EACCES );
    }
  }

  return datadir;
}


char* util_get_fontdir( void ) {

  /* Will use the known OS X directory for fonts, or else try known
   * font directories based on the Filesystem Hierarchy standard.
   */

  char* fontdir;
  char* osxdir = "/Library/Fonts/";
  char* localsharedir = "/usr/local/share/fonts/";
  char* usrsharedir = "/usr/share/fonts/";

  if ( access( osxdir, R_OK | W_OK | X_OK ) == 0 )
    fontdir = osxdir;
  else if ( access( localsharedir, R_OK | W_OK | X_OK ) == 0 )
    fontdir = localsharedir;
  else if ( access( usrsharedir, R_OK | W_OK | X_OK ) == 0 )
    fontdir = usrsharedir;
  else {
    char* username = getlogin();
    if ( username != NULL )
      syslog( LOG_ERR, "Neither the `%s`, `%s`, nor `%s` (OS X only) directories are read-writeable! The `%s` user (or a group it belongs to) need these permissions.", localsharedir, usrsharedir, osxdir, username );
    else
      syslog( LOG_ERR, "Neither the `%s`, `%s`, nor `%s` (OS X only) directories are read-writeable! The user who runs the process [could not be determined] (or a group it belongs to) need these permissions.", localsharedir, usrsharedir, osxdir );

    exit( EACCES );
  }
  
  char* returndir = ( char* ) malloc( ( strlen( fontdir ) + 1 ) );
  strcpy( returndir, fontdir );

  return returndir;
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


int util_power_supply_online( void ) {

  char* powersupplydir = "/sys/class/power_supply/";

  if ( access( powersupplydir, R_OK | X_OK ) == 0 ) {
    char* globpattern = "/sys/class/power_supply/*/online";
    glob_t powersupplies;
    int supplyglob = 1;
    int i = 0;

    int flags = 0;
    flags |= (i > 1 ? GLOB_APPEND : 0);

    supplyglob = glob( globpattern, flags, NULL, & powersupplies );

    if ( supplyglob == 0 ) {
      for ( i = 0; i < powersupplies.gl_pathc; i++ ) {
        int onlinefile = open( powersupplies.gl_pathv[i],  O_RDONLY );
        char powerstatus[1];

        read( onlinefile, powerstatus, 1 );
        close( onlinefile );

        if ( strstr( powerstatus, "1" ) != NULL ) {
          globfree(& powersupplies );
          return 1;
        }
      }

      /* Sources were found but none were online. */
      if ( powersupplies.gl_pathc >= 1 ) {
        globfree(& powersupplies );
        return 0;
      }
    }
    globfree(& powersupplies );
  }

  /* Not POSIX or no info. Assume there is a power source online. */
  return 1;
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

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
#include <stdint.h>

#if defined( _WIN32 ) || defined( _WIN64 )
typedef uint16_t wchar_t;
#endif

#include "utils.h"

void util_single_process_lock( void ) {

#if defined( _WIN32 ) || defined( _WIN64 )
  char *datadir = util_get_datadir( );
  int pidfilepathlen = strlen( datadir ) + strlen( "singleton.lock" ) + 1;
  char *pidfilepath = (char *) calloc( pidfilepathlen, sizeof( char ) );
  strncat( pidfilepath, datadir, pidfilepathlen );
  strncat( pidfilepath, "singleton.lock", pidfilepathlen );

  int pidfile = (int) CreateFile( pidfilepath, GENERIC_READ | GENERIC_WRITE, 0,
                                  NULL, CREATE_ALWAYS, 0, NULL );

  if ( pidfile < 0 )
    exit( EACCES );
#else
  int pidfilepathlen =
      strlen( "/var/run/" ) + strlen( PROGRAM_NAME ) + strlen( ".pid" ) + 1;
  char *pidfilepath = (char *) calloc( pidfilepathlen, sizeof( char ) );

  if ( access( "/run/", R_OK | W_OK | X_OK ) == 0 )
    strncat( pidfilepath, "/run/", pidfilepathlen );
  else if ( access( "/var/run/", R_OK | W_OK | X_OK ) == 0 )
    strncat( pidfilepath, "/var/run/", pidfilepathlen );
  else {
    syslog( LOG_CRIT, "Neither the '/run/' nor '/var/run/' directories are "
                      "read-writable! Exiting." );
    exit( EACCES );
  }

  strncat( pidfilepath, PROGRAM_NAME, pidfilepathlen );
  strncat( pidfilepath, ".pid", pidfilepathlen );

  int pidfilehandle = -1;
  pidfilehandle = open( pidfilepath, O_RDWR | O_CREAT, 0640 );

  if ( pidfilehandle < 0 ) {
    syslog( LOG_ERR, "Could not open '%s'. Exiting.", pidfilepath );
    exit( EACCES );
  }
  if ( flock( pidfilehandle, LOCK_EX | LOCK_NB ) != 0 ) {
    char pidtext[32];
    memset( pidtext, 0, 32 );
    read( pidfilehandle, pidtext, 32 );
    syslog( LOG_ERR, "Could not lock '%s'. It could be locked by another "
                     "process (%ld). Exiting.",
            pidfilepath, strtol( pidtext, 0, 10 ) );
    exit( EACCES );
  }

  char pid[32];
  memset( pid, 0, 32 );
  sprintf( pid, "%d\n", getpid( ) );
  write( pidfilehandle, pid, strlen( pid ) );

  free( pidfilepath );
#endif
}

void util_daemonize_self( void ) {

#if !defined( _WIN32 ) && !defined( _WIN64 )
  pid_t pid, sid;

  /* there is a parent process; therefor I am a child process */
  if ( getppid( ) == 1 )
    return;

  pid = fork( );
  if ( pid < 0 )
    exit( EXIT_FAILURE );
  if ( pid > 0 )
    exit( 0 );

  umask( 0 );

  sid = setsid( );
  if ( sid < 0 )
    exit( EXIT_FAILURE );
  if ( ( chdir( "/" ) ) < 0 )
    exit( EACCES );

  /* Redirect output to nowhere */
  if ( freopen( "/dev/null", "r", stdin ) == NULL )
    perror( "Failed to supress this output when redirecting stdin." );
  if ( freopen( "/dev/null", "w", stdout ) == NULL )
    perror( "Failed to supress this output when redirecting stdout." );
  if ( freopen( "/dev/null", "w", stderr ) == NULL )
    perror( "Failed to supress this output when redirecting stderr." );
#endif
}

void util_init_rand( void ) {

#if !defined( _WIN32 ) && !defined( _WIN64 )
  int random_number;
  int random_file = open( "/dev/urandom", O_RDONLY );

  if ( read( random_file, &random_number, sizeof( random_number ) ) !=
       sizeof( random_number ) )
    perror(
        "Random error. Literally. /dev/urandom did not produce any output." );

  close( random_file );
  srand( random_number );
#endif
}

char *util_get_datadir( void ) {

  char *libdir;

#if defined( _WIN32 ) || defined( _WIN64 )
  char *programdata = getenv( "PROGRAMDATA" );

  int programdatadirlen = strlen( programdata ) + strlen( "\\" ) + 1;

  char *programdatadir = (char *) calloc( programdatadirlen, sizeof( char ) );
  strncat( programdatadir, programdata, programdatadirlen );
  strncat( programdatadir, "\\", programdatadirlen );

  if ( access( programdatadir, R_OK | W_OK ) == 0 )
    libdir = programdatadir;
  else
    exit( EACCES );
#else
  libdir = "/var/lib/";
#endif
  int datadirlen = strlen( libdir ) + strlen( PROGRAM_NAME ) + 2;
  char *datadir = (char *) calloc( datadirlen, sizeof( char ) );

  strncpy( datadir, libdir, datadirlen );

  strncat( datadir, PROGRAM_NAME, datadirlen );
#if defined( _WIN32 ) || defined( _WIN64 )
  strncat( datadir, "\\", datadirlen );
#else
  strncat( datadir, "/", datadirlen );
#endif

  if ( access( datadir, R_OK | W_OK | X_OK ) != 0 ) {
    if ( access( libdir, R_OK | W_OK | X_OK ) == 0 ) {
/* optimistic problem correction */
#if defined( _WIN32 ) || defined( _WIN64 )
      mkdir( datadir );
#else
      mkdir( datadir, 0750 );
#endif
      chmod( datadir, 0750 );
    }
    if ( access( datadir, R_OK | W_OK | X_OK ) != 0 ) {
      exit( EACCES );
    }
  }

  return datadir;
}

char *util_get_fontdir( void ) {

  /* Will use the known OS X directory for fonts, or else try known
   * font directories based on the Filesystem Hierarchy standard.
   */

  char *fontdir;

#if defined( _WIN32 ) || defined( _WIN64 )
  char *sysroot = getenv( "SYSTEMROOT" );
  int win32dirlen = strlen( sysroot ) + strlen( "\\Fonts\\" ) + 1;
  char *win32dir = (char *) calloc( win32dirlen, sizeof( char ) );

  strncat( win32dir, sysroot, win32dirlen );
  strncat( win32dir, "\\Fonts\\", win32dirlen );

  if ( access( win32dir, R_OK | W_OK ) == 0 )
    fontdir = win32dir;
  else
    exit( EACCES );
#else

  char *osxdir = "/Library/Fonts/";
  char *localsharedir = "/usr/local/share/fonts/";
  char *usrsharedir = "/usr/share/fonts/";

  if ( access( osxdir, R_OK | W_OK | X_OK ) == 0 )
    fontdir = osxdir;
  else if ( access( localsharedir, R_OK | W_OK | X_OK ) == 0 )
    fontdir = localsharedir;
  else if ( access( usrsharedir, R_OK | W_OK | X_OK ) == 0 )
    fontdir = usrsharedir;
  else {
    char *username = getlogin( );
    if ( username != NULL )
      syslog( LOG_ERR, "Neither the `%s`, `%s`, nor `%s` (OS X only) "
                       "directories are read-writeable! The `%s` user (or a "
                       "group it belongs to) need these permissions.",
              localsharedir, usrsharedir, osxdir, username );
    else
      syslog( LOG_ERR, "Neither the `%s`, `%s`, nor `%s` (OS X only) "
                       "directories are read-writeable! The user who runs the "
                       "process [could not be determined] (or a group it "
                       "belongs to) need these permissions.",
              localsharedir, usrsharedir, osxdir );

    exit( EACCES );
  }

#endif

  int returndirlen = strlen( fontdir ) + 1;
  char *returndir = (char *) calloc( returndirlen, sizeof( char ) );
  strncpy( returndir, fontdir, returndirlen );

  return returndir;
}

void util_uninstall_all_fonts( char *fontsetlist, char *fontdir ) {

  if ( access( fontsetlist, F_OK | R_OK ) == 0 ) {
    FILE *fontsetlist_file = fopen( fontsetlist, "r" );
    char fontname[200];

    while ( fgets( fontname, 200, fontsetlist_file ) != NULL ) {

      /* Shouldn't ever be any slashes in this string */
      if ( strstr( fontname, "\\" ) != NULL ||
           strstr( fontname, "/" ) != NULL ) {
        continue;
      }

      /* Remove any line endings from the font name */
      char *pos;
      if ( ( pos = strchr( fontname, '\r' ) ) != NULL )
        *pos = '\0';
      if ( ( pos = strchr( fontname, '\n' ) ) != NULL )
        *pos = '\0';

      int pathlen = strlen( fontdir ) + strlen( fontname ) +
                    strlen( FONTFILE_EXTENSION ) + 1;

      char *path = calloc( pathlen, sizeof( char ) );

      strncat( path, fontdir, pathlen );
      strncat( path, fontname, pathlen );
      strncat( path, FONTFILE_EXTENSION, pathlen );

      unlink( path );
      free( path );

#if defined( _WIN32 ) || defined( _WIN64 )
      char *hKeyFontsDir =
          "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts";
      int hKeyValuePathlen = strlen( fontname ) + strlen( " (TrueType)" ) + 1;
      char *hKeyValuePath = calloc( hKeyValuePathlen, sizeof( char ) );

      strncat( hKeyValuePath, fontname, hKeyValuePathlen );
      strncat( hKeyValuePath, " (TrueType)", hKeyValuePathlen );

      HKEY fontskey;
#ifdef _WIN32
      RegOpenKeyExA( HKEY_LOCAL_MACHINE, hKeyFontsDir, 0,
                     KEY_WOW64_32KEY | KEY_SET_VALUE | KEY_ALL_ACCESS,
                     &fontskey );
#endif
#ifdef _WIN64
      RegOpenKeyExA( HKEY_LOCAL_MACHINE, hKeyFontsDir, 0,
                     KEY_WOW64_64KEY | KEY_SET_VALUE | KEY_ALL_ACCESS,
                     &fontskey );
#endif
      if ( fontskey != NULL ) {
        /* status is unused but resolves a Heisenbug */
        int status = (int) RegDeleteValueA( fontskey, hKeyValuePath );

        RegCloseKey( fontskey );
      }

      free( hKeyValuePath );
#endif
    }

    fclose( fontsetlist_file );
    unlink( fontsetlist );
  }
}

int util_power_supply_online( void ) {

#if defined( _WIN32 ) || defined( _WIN64 )
  return -1;
#else

  char *powersupplydir = "/sys/class/power_supply/";

  if ( access( powersupplydir, R_OK | X_OK ) == 0 ) {
    char *globpattern = "/sys/class/power_supply/*/online";
    glob_t powersupplies;
    int supplyglob = 1;
    int i = 0;

    int flags = 0;
    flags |= ( i > 1 ? GLOB_APPEND : 0 );

    supplyglob = glob( globpattern, flags, NULL, &powersupplies );

    if ( supplyglob == 0 ) {
      for ( i = 0; i < powersupplies.gl_pathc; i++ ) {
        int onlinefile = open( powersupplies.gl_pathv[i], O_RDONLY );
        char powerstatus[1];

        read( onlinefile, powerstatus, 1 );
        close( onlinefile );

        if ( strstr( powerstatus, "1" ) != NULL ) {
          globfree( &powersupplies );
          return 1;
        }
      }

      /* Sources were found but none were online. */
      if ( powersupplies.gl_pathc >= 1 ) {
        globfree( &powersupplies );
        return 0;
      }
    }
    globfree( &powersupplies );
  }

  /* Not POSIX or no info. Assume there is a power source online. */
  return 1;
#endif
}

int util_maxpow2( int val ) {

  if ( val == 1 )
    return 0;

  int result = 1;
  while ( result <= val )
    result = result << 1;

  return result >> 1;
}

uint8_t *util_charptr_to_usc2be( char *str ) {

  size_t str_len = strlen( str );
  size_t usc2be_buffer_len = 2 * str_len;
  uint8_t *usc2be_buffer = malloc( usc2be_buffer_len + 1 );
  *usc2be_buffer = '\0';

  /* Convert from ISO8859-15 to UCS2 big-endian (specifying endianness removes
   * the BOM) */
  for ( int i = 0; i < str_len; i++ ) {
    char letter = str[i];

    /* Check for allowed characters */
    if ( letter != ' ' && letter != '-' && letter != '(' && letter != ')' &&
         !( letter >= 'a' && letter <= 'z' ) &&
         !( letter >= 'A' && letter <= 'Z' ) ) {
      letter = 'a';
    }

    usc2be_buffer[i * 2] = 0;
    usc2be_buffer[i * 2 + 1] = letter;
  }

  return usc2be_buffer;
}

#if defined( _WIN32 ) || defined( _WIN64 )
int util_winrand( ) {

  int rand_int;
  unsigned int rand_out;

  rand_s( &rand_out );
  rand_int = (int) ( ( (double) rand_out / ( (double) UINT_MAX + 1 ) *
                       (double) INT_MAX ) ) +
             1;

  return rand_int;
}
#endif
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

#include "opentype.h"

BUFFER *assemble_opentype_font( struct names *font_names ) {

  /* The buffer for the font file (main buffer) */
  BUFFER *fontbuffer = makebuffer( BUFSIZE );

  /* Offset table (identifier)
   * Most values are set after creating all other tables
   */
  OTF_OFFSET_TABLE *offset_table =
      buffer_alloc( fontbuffer, sizeof( OTF_OFFSET_TABLE ) );
  memcpy( &offset_table->versiontag.tag, "OTTO", 4 );

  /* Table record entries
   * Sorted alphabetically (recommended order).
   * ->offset and ->length is set when tables are inserted.
   */
  size_t before_tables = fontbuffer->position;
  OTF_TABLE_RECORD *tablerecord_cff =
      alloc_table_record_entry( fontbuffer, "CFF " );
  OTF_TABLE_RECORD *tablerecord_os2 =
      alloc_table_record_entry( fontbuffer, "OS/2" );
  OTF_TABLE_RECORD *tablerecord_cmap =
      alloc_table_record_entry( fontbuffer, "cmap" );
  OTF_TABLE_RECORD *tablerecord_head =
      alloc_table_record_entry( fontbuffer, "head" );
  OTF_TABLE_RECORD *tablerecord_hhea =
      alloc_table_record_entry( fontbuffer, "hhea" );
  OTF_TABLE_RECORD *tablerecord_hmtx =
      alloc_table_record_entry( fontbuffer, "hmtx" );
  OTF_TABLE_RECORD *tablerecord_maxp =
      alloc_table_record_entry( fontbuffer, "maxp" );
  OTF_TABLE_RECORD *tablerecord_name =
      alloc_table_record_entry( fontbuffer, "name" );
  OTF_TABLE_RECORD *tablerecord_post =
      alloc_table_record_entry( fontbuffer, "post" );
  size_t after_tables = fontbuffer->position;

  struct cff_result *cff_res = set_table_cff( font_names );
  BUFFER *cff_buffer = cff_res->cffData;

  OTF_TABLE_HEAD *table_head =
      insert_full_table( fontbuffer, tablerecord_head, set_table_head( ) );
  insert_full_table( fontbuffer, tablerecord_hhea, set_table_hhea( ) );
  insert_full_table( fontbuffer, tablerecord_maxp,
                     set_table_maxp( cff_res->indexChars->count ) );
  insert_full_table( fontbuffer, tablerecord_os2, set_table_os2( font_names ) );
  insert_full_table( fontbuffer, tablerecord_hmtx, set_table_hmtx( ) );
  insert_full_table( fontbuffer, tablerecord_name,
                     set_table_name( font_names ) );
  insert_full_table( fontbuffer, tablerecord_cmap, set_table_cmap( ) );
  insert_full_table( fontbuffer, tablerecord_post, set_table_post( ) );
  insert_full_table( fontbuffer, tablerecord_cff, cff_buffer );
  free( cff_res );

  /* Set the table records (immediately following the offset table [below]) */
  buffer_alloc( fontbuffer,
                4 - ( fontbuffer->position % 4 ) ); /* Pad to a multiple of 4 */
  set_table_records( fontbuffer, tablerecord_cff );
  set_table_records( fontbuffer, tablerecord_head );
  set_table_records( fontbuffer, tablerecord_hhea );
  set_table_records( fontbuffer, tablerecord_hmtx );
  set_table_records( fontbuffer, tablerecord_maxp );
  set_table_records( fontbuffer, tablerecord_name );
  set_table_records( fontbuffer, tablerecord_os2 );
  set_table_records( fontbuffer, tablerecord_cmap );
  set_table_records( fontbuffer, tablerecord_post );

  /* Set the offset table (identifier) at the start of the file */
  offset_table->numTables =
      htons( ( uint16_t )( after_tables - before_tables ) /
             sizeof( OTF_TABLE_RECORD ) );
  offset_table->searchRange =
      htons( (uint16_t) util_maxpow2( ntohs( offset_table->numTables ) * 16 ) );
  offset_table->entrySelector = htons(
      (uint16_t) log2( util_maxpow2( ntohs( offset_table->numTables ) ) ) );
  offset_table->rangeShift = htons( ntohs( offset_table->numTables ) * 16 -
                                    ntohs( offset_table->searchRange ) );

  /* Set the file checksum in the HEAD table
   * Calculated by subtracting the checksum for the entire file from a magical
   * constant.
   */
  table_head->checkSumAdjustment =
      htonl( 0xB1B0AFBA -
             calc_table_checksum( fontbuffer->data, fontbuffer->position ) );

  return fontbuffer;
}

void *insert_full_table( BUFFER *complete_font, OTF_TABLE_RECORD *head_entry,
                         BUFFER *insert_me ) {
  buffer_alloc( complete_font, 4 - ( complete_font->position %
                                     4 ) ); /* Pad to a multiple of 4 */

  void *table = &complete_font->data[complete_font->position];
  head_entry->offset = complete_font->position;
  head_entry->length = insert_me->position;
  insert_buffer( complete_font, insert_me );
  free_buffer( insert_me );
  return table;
}

int font_generator( void ) {

  int failed = 0;

  /* Assign a random font name */
  struct names *font_names = make_fontnames( );

  /* Where to store the file */
  int filenamelen =
      strlen( font_names->postscriptName ) + strlen( FONTFILE_EXTENSION ) + 1;
  char *filename = (char *) calloc( filenamelen, sizeof( char ) );
  strncat( filename, font_names->postscriptName, filenamelen );
  strncat( filename, FONTFILE_EXTENSION, filenamelen );

  int filepathlen = strlen( fontdir ) + strlen( filename ) + 1;
  char *filepath = (char *) calloc( filepathlen, sizeof( char ) );
  strncpy( filepath, fontdir, strlen( fontdir ) );
  strncat( filepath, filename, filepathlen );

  /* Font output file exists. Never be destructive. */
  if ( access( filepath, F_OK ) != 0 ) {

    /* Assemble the actual font work */
    BUFFER *fontbuffer = assemble_opentype_font( font_names );

/* Write out the buffer to the font file directly to installation directory */
#if defined( _WIN32 ) || defined( _WIN64 )
    int otf_output_file =
        (int) CreateFile( filepath, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                          CREATE_ALWAYS, 0, NULL );
#else
    int otf_output_file = open( filepath, O_RDWR | O_CREAT | O_EXCL,
                                S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
#endif

    if ( otf_output_file >= 0 ) {

/* Record font installation to install log */
#if defined( _WIN32 ) || defined( _WIN64 )
      int font_list_file = _open( fontsetlist, _O_RDWR | _O_CREAT | _O_APPEND,
                                  _S_IREAD | _S_IWRITE );
#else
      int font_list_file = open( fontsetlist, O_RDWR | O_CREAT | O_APPEND,
                                 S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );
#endif
      if ( font_list_file >= 0 ) {
        int otf_output_file_written;

#if defined( _WIN32 ) || defined( _WIN64 )
        otf_output_file_written = WriteFile( otf_output_file, fontbuffer->data,
                                             fontbuffer->position, NULL, NULL );
        /* Cheating, but sufficient here. */
        if ( otf_output_file_written > 0 )
          otf_output_file_written = fontbuffer->position;
#else
        otf_output_file_written =
            write( otf_output_file, fontbuffer->data, fontbuffer->position );
#endif

        /* Work-around for FontCache being overly eager and locking new fonts
           while they’re being written */
        if ( otf_output_file_written != fontbuffer->position ) {
          util_really_remove_file( filepath );
          otf_output_file_written =
              write( otf_output_file, fontbuffer->data, fontbuffer->position );
          if ( otf_output_file_written != fontbuffer->position ) {
            util_really_remove_file( filepath );
            failed = 1;
          }
        }

        int font_list_file_written;

        if ( failed == 0 ) {
          font_list_file_written =
              write( font_list_file, font_names->postscriptName,
                     strlen( font_names->postscriptName ) );
          write( font_list_file, "\n", 1 );
          if ( font_list_file_written !=
               strlen( font_names->postscriptName ) ) {
            util_really_remove_file( filepath );
            failed = 1;
          }

#if defined( _WIN32 ) || defined( _WIN64 )
          if ( failed == 0 ) {

            char *hKeyFontsDir =
                "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts";

            int hKeyValuePathlen = strlen( font_names->postscriptName ) +
                                   strlen( " (TrueType)" ) + 1;
            char *hKeyValuePath = calloc( hKeyValuePathlen, sizeof( char ) );

            strncat( hKeyValuePath, font_names->postscriptName,
                     hKeyValuePathlen );
            strncat( hKeyValuePath, " (TrueType)", hKeyValuePathlen );

            HKEY fontskey;
#ifdef _WIN32
            RegOpenKeyExA( HKEY_LOCAL_MACHINE, hKeyFontsDir, 0,
                           KEY_WOW64_32KEY | KEY_ALL_ACCESS, &fontskey );
#endif
#ifdef _WIN64
            RegOpenKeyExA( HKEY_LOCAL_MACHINE, hKeyFontsDir, 0,
                           KEY_WOW64_64KEY | KEY_ALL_ACCESS, &fontskey );
#endif

            if ( fontskey != NULL ) {
              RegSetValueExA( fontskey, hKeyValuePath, 0, REG_SZ, filename,
                              strlen( filename ) + 1 );
              RegCloseKey( fontskey );
            } else {
              /* Can't register the font, remove it. */
              util_really_remove_file( filepath );
            }

            free( hKeyValuePath );
          }
#endif
        }
      }

      close( font_list_file );

#if defined( _WIN32 ) || defined( _WIN64 )
      CloseHandle( otf_output_file );
#else
      close( otf_output_file );
#endif
    }

    free_buffer( fontbuffer );
  }

  /* Free the last buffers */
  free( font_names->fontFamilyName );
  free( font_names->uniqueFontName );
  free( font_names->fullFontName );
  free( font_names->postscriptName );
  free( font_names->foundryVendorID );
  free( font_names );

  free( filepath );

  return failed;
}

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


#include "opentype.h"


BUFFER *assemble_opentype_font( struct names *font_names ) {

  /* The buffer for the font file (main buffer) */
  BUFFER *fontbuffer = makebuffer( BUFSIZE );

  /* Offset table (identifier)
   * Most values are set after creating all other tables
   */
  OTF_OFFSET_TABLE *offset_table = buffer_alloc( fontbuffer, sizeof( OTF_OFFSET_TABLE ) );
  memcpy( &offset_table->versiontag.tag, "OTTO", 4 );

  /* Table record entries
   * Sorted alphabetically (recommended order).
   * ->offset and ->length is set when tables are inserted.
   */
  size_t before_tables = fontbuffer->position;
  OTF_TABLE_RECORD *tablerecord_cff  = alloc_table_record_entry( fontbuffer, "CFF " );
  OTF_TABLE_RECORD *tablerecord_os2  = alloc_table_record_entry( fontbuffer, "OS/2" );
  OTF_TABLE_RECORD *tablerecord_cmap = alloc_table_record_entry( fontbuffer, "cmap" );
  OTF_TABLE_RECORD *tablerecord_head = alloc_table_record_entry( fontbuffer, "head" );
  OTF_TABLE_RECORD *tablerecord_hhea = alloc_table_record_entry( fontbuffer, "hhea" );
  OTF_TABLE_RECORD *tablerecord_hmtx = alloc_table_record_entry( fontbuffer, "hmtx" );
  OTF_TABLE_RECORD *tablerecord_maxp = alloc_table_record_entry( fontbuffer, "maxp" );
  OTF_TABLE_RECORD *tablerecord_name = alloc_table_record_entry( fontbuffer, "name" );
  OTF_TABLE_RECORD *tablerecord_post = alloc_table_record_entry( fontbuffer, "post" );
  size_t after_tables = fontbuffer->position;


  struct cff_result *cff_res = set_table_cff( font_names );
  BUFFER *cff_buffer = cff_res->cffData;

  OTF_TABLE_HEAD *table_head =  insert_full_table( fontbuffer, tablerecord_head, set_table_head() );
  insert_full_table( fontbuffer, tablerecord_hhea, set_table_hhea() );
  insert_full_table( fontbuffer, tablerecord_maxp, set_table_maxp( cff_res->indexChars->count ) );
  insert_full_table( fontbuffer, tablerecord_os2 , set_table_os2( font_names ) );
  insert_full_table( fontbuffer, tablerecord_hmtx, set_table_hmtx() );
  insert_full_table( fontbuffer, tablerecord_name, set_table_name( font_names ) );
  insert_full_table( fontbuffer, tablerecord_cmap, set_table_cmap() );
  insert_full_table( fontbuffer, tablerecord_post, set_table_post());
  insert_full_table( fontbuffer, tablerecord_cff, cff_buffer);
  free( cff_res );

  /* Set the table records (immediately following the offset table [below]) */
  buffer_alloc( fontbuffer, 4 - ( fontbuffer->position % 4 ) ); /* Pad to a multiple of 4 */
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
  offset_table->numTables =     htons( ( uint16_t ) ( after_tables - before_tables ) / sizeof( OTF_TABLE_RECORD ) );
  offset_table->searchRange =   htons( ( uint16_t ) util_maxpow2( ntohs( offset_table->numTables ) * 16 ) );
  offset_table->entrySelector = htons( ( uint16_t ) log2( util_maxpow2( ntohs(offset_table->numTables ) ) ) );
  offset_table->rangeShift =    htons( ntohs( offset_table->numTables ) * 16 - ntohs( offset_table->searchRange ) );

  /* Set the file checksum in the HEAD table
   * Calculated by subtracting the checksum for the entire file from a magical constant.
   */
  table_head->checkSumAdjustment = htonl( 0xB1B0AFBA - calc_table_checksum( fontbuffer->data, fontbuffer->position ) );

  return fontbuffer;
}


void *insert_full_table( BUFFER *complete_font, OTF_TABLE_RECORD *head_entry, BUFFER *insert_me ) {
	buffer_alloc( complete_font, 4 - ( complete_font->position % 4 ) ); /* Pad to a multiple of 4 */

	void *table = &complete_font->data[complete_font->position];
	head_entry->offset = complete_font->position;
	head_entry->length = insert_me->position;
	insert_buffer( complete_font, insert_me );
	free_buffer( insert_me );
	return table;
}


int font_generator( void ) {

  /* Assign a random font name */
  struct names *font_names = make_fontnames();

  /* Where to store the file */
  char *filepath = ( char* ) calloc( ( strlen( fontdir ) + strlen( font_names->postscriptName ) + strlen ( FONTFILE_EXTENSION ) + 1 ), sizeof( char ) );
  strncpy( filepath, fontdir, strlen( fontdir ) );
  strcat( filepath, font_names->postscriptName );
  strcat( filepath, FONTFILE_EXTENSION );

  /* Font output file exists. Never be destructive. */
  if ( access( filepath, F_OK ) == 0 ) {
    return EEXIST;
  }

  /* Assemble the actual font work */
  BUFFER *fontbuffer = assemble_opentype_font( font_names );

  /* Write out the buffer to the font file directly to installation directory */
  int otf_output_file = open( filepath, O_RDWR|O_CREAT|O_EXCL, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH );

  if ( write( otf_output_file, fontbuffer->data, fontbuffer->position ) == fontbuffer->position ) {

    /* Record font installation to install log */
    int font_list_file = open( fontsetlist, O_RDWR|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP );
    ssize_t font_list_file_writesize = write( font_list_file, font_names->postscriptName, strlen( font_names->postscriptName ) );
    ssize_t font_list_file_writenlsize = write( font_list_file, "\n", 1 );

    if ( ( font_list_file_writesize + font_list_file_writenlsize ) != ( strlen( font_names->postscriptName ) + 1 ) ) {
      syslog( LOG_ERR, "Failed to log successful installation of %s.%s in %s. The font was installed but will not be removed as part of normal operation.", font_names->postscriptName, FONTFILE_EXTENSION, fontsetlist );
    }

    close( font_list_file );
    close( otf_output_file );
  }

  else {
    /* something went wrong with the write, delete the font */
    close(  otf_output_file );
    unlink( filepath );
  }

  /* Free the last buffers */
  free( font_names->fontFamilyName );
  free( font_names->uniqueFontName );
  free( font_names->fullFontName );
  free( font_names->postscriptName );
  free( font_names->foundryVendorID );
  free( font_names );
  free_buffer( fontbuffer );
  free( filepath );

  return 0;
}

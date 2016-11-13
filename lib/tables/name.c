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


#include "name.h"


BUFFER *set_table_name( struct names *font_names ) {

  uint32_t records_in_total = 16; /* Eight name records in two formats. */
  BUFFER *name_buf = makebuffer( sizeof( OTF_TABLE_NAME ) );
  BUFFER *rec_buf = makebuffer( sizeof( NAME_RECORD ) * records_in_total );
  BUFFER *stringbuf = makebuffer( BUFSIZE );
  BUFFER *return_buf = makebuffer( BUFSIZE );

  OTF_TABLE_NAME *t_nametable = buffer_alloc( name_buf, sizeof( OTF_TABLE_NAME ) );
  t_nametable->format = 0;
  t_nametable->count = htons( records_in_total );
  t_nametable->stringOffset = htons( sizeof( OTF_TABLE_NAME ) + ( sizeof( NAME_RECORD ) * records_in_total ) );

  put_namerecord( rec_buf, stringbuf, ( char* ) font_names->copyrightNotice, 1, 0, 0, 0 );
  put_namerecord( rec_buf, stringbuf, ( char* ) font_names->fontFamilyName, 1, 0, 0, 1 );
  put_namerecord( rec_buf, stringbuf, ( char* ) font_names->fontFamilyStyle, 1, 0, 0, 2 );
  put_namerecord( rec_buf, stringbuf, ( char* ) font_names->uniqueFontName, 1, 0, 0, 3 );
  put_namerecord( rec_buf, stringbuf, ( char* ) font_names->fullFontName, 1, 0, 0, 4 );
  put_namerecord( rec_buf, stringbuf, ( char* ) font_names->versionString, 1, 0, 0, 5 );
  put_namerecord( rec_buf, stringbuf, ( char* ) font_names->postscriptName, 1, 0, 0, 6 );
  put_namerecord( rec_buf, stringbuf, ( char* ) font_names->foundryVendorID, 1, 0, 0, 8 );

  put_namerecord_w( rec_buf, stringbuf, ( char* ) font_names->copyrightNotice, 3, 1, 1033, 0 );
  put_namerecord_w( rec_buf, stringbuf, ( char* ) font_names->fontFamilyName, 3, 1, 1033, 1 );
  put_namerecord_w( rec_buf, stringbuf, ( char* ) font_names->fontFamilyStyle, 3, 1, 1033, 2 );
  put_namerecord_w( rec_buf, stringbuf, ( char* ) font_names->uniqueFontName, 3, 1, 1033, 3 );
  put_namerecord_w( rec_buf, stringbuf, ( char* ) font_names->fullFontName, 3, 1, 1033, 4 );
  put_namerecord_w( rec_buf, stringbuf, ( char* ) font_names->versionString, 3, 1, 1033, 5 );
  put_namerecord_w( rec_buf, stringbuf, ( char* ) font_names->postscriptName, 3, 1, 1033, 6 );
  put_namerecord_w( rec_buf, stringbuf, ( char* ) font_names->foundryVendorID , 3, 1, 1033, 8 );

  insert_buffer( return_buf, name_buf );
  free_buffer( name_buf );
  insert_buffer( return_buf, rec_buf );
  free_buffer( rec_buf );
  insert_buffer( return_buf, stringbuf );
  free_buffer( stringbuf );

  return return_buf;
}


void put_namerecord( BUFFER *rec_buf, BUFFER *stringbuf, char *name, uint16_t platformID, uint16_t encodingID, uint16_t languageID, uint16_t nameID ) {

  int length = strlen( name );
  char *str;
  NAME_RECORD *nr = buffer_alloc( rec_buf, sizeof( NAME_RECORD ) );

  nr->length = htons( length );
  nr->offset = htons( stringbuf->position );
  nr->platformID = htons( platformID );
  nr->encodingID = htons( encodingID );
  nr->languageID = htons( languageID );
  nr->nameID = htons( nameID );
  str = buffer_alloc( stringbuf, length + 1 );
  memcpy( str, name, length );
}

void put_namerecord_w( BUFFER *rec_buf, BUFFER *stringbuf, char *name, uint16_t platformID, uint16_t encodingID, uint16_t languageID, uint16_t nameID ) {

  char *name_victim = name;
  size_t size_org = strlen( name );
  size_t inbytesleft = size_org;
  size_t outbytes_org = 2*size_org + 2;
  size_t outbytesleft = outbytes_org;
  char *converted = malloc( outbytes_org );
  char *converted_victim = converted;

  /* Convert from ISO8859-15 to UCS2 big-endian ( specifying endianness removes the BOM ),
     replacing untranslateable characters with similar ones ( but there shouldn't be any ),
  */
  iconv_t iconv_settings = iconv_open( "UCS-2BE//TRANSLIT", "ISO8859-15" );
  if ( iconv_settings == ( iconv_t ) -1 )   perror( "iconv_open failure : " );
  /* second argument should be (const char**) on FreeBSD but (char**) is expected on Linuses and OS X. FreeBSD must live with the compile warning. */
  size_t iconv_res = iconv( iconv_settings, ( char** ) &name_victim, &inbytesleft, &converted_victim, &outbytesleft );
  iconv_close( iconv_settings );
  size_t converted_bytes = outbytes_org - outbytesleft;
  if ( iconv_res != 0 )  perror( "iconv error : " );

  NAME_RECORD *nr = buffer_alloc( rec_buf, sizeof( NAME_RECORD ) );

  nr->length = htons( converted_bytes );
  nr->offset = htons( stringbuf->position );
  nr->platformID = htons( platformID );
  nr->encodingID = htons( encodingID );
  nr->languageID = htons( languageID );
  nr->nameID = htons( nameID );
  char *str = buffer_alloc( stringbuf, converted_bytes + 2 );

  memcpy( str, converted, converted_bytes );
  free( converted );
}

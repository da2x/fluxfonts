/*

  Fluxfonts – a continual font generator for increased privacy
  Copyright 2012–2017, Daniel Aleksandersen
  Copyright 2012, Daniel Nebdal
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

#include "cff.h"

struct cff_result *set_table_cff( struct names *font_names ) {
  struct cff_result *result =
      (struct cff_result *) malloc( sizeof( struct cff_result ) );
  BUFFER *cff_buffer = makebuffer( BUFSIZE );

  CFF_HEADER *cff_header = buffer_alloc( cff_buffer, sizeof( CFF_HEADER ) );
  cff_header->majorVersion = 1;
  cff_header->minorVersion = 0;
  cff_header->headerSize = 4;
  cff_header->offsetSize = 1;

  CFF_INDEX_SINGLE_ENTRY *cff_index_name =
      buffer_alloc( cff_buffer, sizeof( CFF_INDEX_SINGLE_ENTRY ) );
  cff_index_name->count = htons( 1 );
  cff_index_name->offsetSize = 1;
  cff_index_name->offset = 1;
  cff_index_name->length = strlen( font_names->postscriptName ) + 1;
  copy_string_to_buffer( cff_buffer, font_names->postscriptName );

  CFF_INDEX_SINGLE_ENTRY *cff_index_dict =
      buffer_alloc( cff_buffer, sizeof( CFF_INDEX_SINGLE_ENTRY ) );
  cff_index_dict->count = htons( 1 );
  cff_index_dict->offsetSize = 1;
  cff_index_dict->offset = 1;

  /* There are 390 standard strings.
  * String IDs above 390 refer to string N-390 in the index.
  * (That is: 391 is the first, etc.) */
  uint8_t magic1[] = {
      0xf8, 0x1b,       /*  391 : String 0 */
      0x00,             /*    0 : Operator "version"*/
      0xf8, 0x1c,       /*  392 : String 1 */
      0x02,             /*    2 : Operator "FullName" */
      0xf8, 0x1d,       /*  393 : String 2 */
      0x03,             /*    3 : Operator "FamilyName" */
      0xf8, 0x19,       /*  389 : "Roman" */
      0x04,             /*    4 : Operator "Weight" */
      0x1c, 0x6f, 0x00, /* 28416 */
      0x0d,             /*   13 : Operator "UniqueID" */
      0xfb, 0x3c,       /* -168 */
      0xfb, 0x6e,       /* -218 */
      0xfa, 0x7c,       /* 1000 */
      0xfa, 0x16,       /*  898 */
      0x05,             /*    5 : Operator "FontBBox" */
  };

  /* Private dict, as specified in the Compact Font Format.
   * Constant, since it only describes aspects of the glyphs.
   */
  uint8_t magic2[] = {
      0x7d,       /*  -14 */
      0x99,       /*   14 */
      0xf9, 0x2a, /*  662 */
      0x99,       /*   14 */
      0xfb, 0x76, /* -226 */
      0x95,       /*   10 */
      0xf7, 0x73, /*  223 */
      0x8b,       /*    0 */
      0x06,       /*  BlueValues */
      0xf7, 0x9a, /*  262 */
      0x93,       /*    8 */
      0xfc, 0x7c, /* -488 */
      0x8c,       /*    1 */
      0x07,       /*  OtherBlues */
      0x7d,       /*  -14 */
      0x99,       /*   14 */
      0xf8, 0x56, /*  450 */
      0x95,       /*   10 */
      0xf7, 0x5e, /*  202 */
      0x99,       /*   14 */
      0x08,       /*  FamilyBlues */
      0xfb, 0x6e, /* -218 */
      0x8c,       /*    1 */
      0xf8, 0x73, /*  479 */
      0x93,       /*    8 */
      0xf7, 0x10, /*  124 */
      0x8b,       /*    0 */
      0x09,       /*  FamilyOtherBlues */
      0xa7,       /*   28 */
      0x0a,       /*  StdHW */
      0xdf,       /*   84 */
      0x0b,       /*  StdVW */
      0xf7, 0x1f, /*  139 */
      0x14        /*  defaultWidthX */
  };
  uint8_t *magic_here = buffer_alloc( cff_buffer, sizeof( magic1 ) );
  memcpy( magic_here, magic1, sizeof( magic1 ) );

  PRIVATE_DICT_OFFSETS pd_offsets;
  pd_offsets.charstrings_pos =
      cff_buffer->position + sizeof( CFF_INDEX_STRING ) + strlen( "001.007" ) +
      strlen( font_names->fullFontName ) +
      strlen( font_names->fontFamilyName ) + sizeof( CFF_INDEX_EMPTY );

  pd_offsets.private_pos =
      pd_offsets.charstrings_pos + sizeof( CFF_INDEX_CHARS ) + 4;
  pd_offsets.encoding_pos = pd_offsets.private_pos + sizeof( magic2 );
  pd_offsets.charset_pos = pd_offsets.encoding_pos + 5;
  pd_offsets.private_len = sizeof( magic2 );

  BUFFER *topdict_offsets = make_topdict_offsets( &pd_offsets );
  insert_buffer( cff_buffer, topdict_offsets );
  cff_index_dict->length = sizeof( magic1 ) + topdict_offsets->position + 1;
  free_buffer( topdict_offsets );

  CFF_INDEX_STRING *cff_index_string =
      buffer_alloc( cff_buffer, sizeof( CFF_INDEX_STRING ) );
  cff_index_string->count = htons( 3 );
  cff_index_string->offsetSize = 1;
  cff_index_string->offset1 = 1;
  cff_index_string->offset2 = strlen( "001.007" ) + cff_index_string->offset1;
  cff_index_string->offset3 =
      strlen( font_names->fullFontName ) + cff_index_string->offset2;
  cff_index_string->length =
      cff_index_string->offset3 + strlen( font_names->fontFamilyName );
  copy_string_to_buffer(
      cff_buffer,
      "001.007" ); /* SID for "space" and "ampersand" replace 7 with 0 for
                      ".notdef", maybe? */
  copy_string_to_buffer( cff_buffer, font_names->fullFontName );
  copy_string_to_buffer( cff_buffer, font_names->fontFamilyName );

  CFF_INDEX_EMPTY *cff_index_gsubrs =
      buffer_alloc( cff_buffer, sizeof( CFF_INDEX_EMPTY ) );
  cff_index_gsubrs->count = htons( 0 );

  CFF_INDEX_CHARS *cff_index_chars =
      buffer_alloc( cff_buffer, sizeof( CFF_INDEX_CHARS ) );
  cff_index_chars->count = htons( 4 );
  cff_index_chars->offsetSize = 1;
  cff_index_chars->offset1 = 1;
  cff_index_chars->offset2 = 2;
  cff_index_chars->offset3 = 3;
  cff_index_chars->offset4 = 4;
  cff_index_chars->length = 5;

  /* Fill in two end characters in the buffer
  *  Recommended as in http://www.microsoft.com/typography/otspec/recom.htm
  * (First Four Glyphs in Fonts)
  * .notdef, .null, CR, and space
  */
  uint16_t *endchars = buffer_alloc( cff_buffer, 2 );
  uint16_t *endchars2 = buffer_alloc( cff_buffer, 2 );
  *endchars = 0x0e0e;
  *endchars2 = 0x0e0e;

  /* Private dict here */
  uint8_t *magic2_here = buffer_alloc( cff_buffer, sizeof( magic2 ) );
  memcpy( magic2_here, magic2, sizeof( magic2 ) );

  /* encoding */
  buffer_alloc( cff_buffer, 1 ); /* Format = 0 */
  uint8_t *encodingCount = buffer_alloc( cff_buffer, 1 );
  *encodingCount = 3;
  uint8_t *encoding = buffer_alloc( cff_buffer, 3 );
  encoding[0] = 0;    /* null */
  encoding[1] = '\n'; /* CR */
  encoding[2] = ' ';  /* space */

  /* character set */
  buffer_alloc( cff_buffer, 0 ); /* format = 0 */
  uint16_t *charset_target = buffer_alloc( cff_buffer, 4 * sizeof( uint16_t ) );
  uint16_t charset[4] = {0, 0, 0, 0};
  memcpy( charset_target, charset, 4 * sizeof( uint16_t ) );

  result->cffData = cff_buffer;
  result->indexChars = cff_index_chars;

  return result;
}

int get_encoded_charstring_len( int number ) {

  int length = 5;
  if ( number >= -107 && number <= 107 )
    length = 1;
  else if ( number >= 108 && number <= 1131 )
    length = 2;
  else if ( number >= -1131 && number <= -108 )
    length = 2;
  else if ( number >= -32768 && number <= 32767 )
    length = 3;

  return length;
}

/* Encode an integer as a charstring number, as defined in the Type 1 Font
 * Format */
BUFFER *encode_charstring_number( int32_t number ) {

  int length = get_encoded_charstring_len( number );

  BUFFER *encoded = makebuffer( length );

  if ( length == 1 ) {
    put_int8( encoded, number + 139 );
  }

  if ( length == 2 && number < 0 ) {
    int absv = number * -1;
    int num1 = ( absv - 108 ) / 256;
    int num2 = ( absv - 108 ) % 256;

    put_int8( encoded, num1 + 251 );
    put_int8( encoded, num2 );
  }

  if ( length == 2 && number > 0 ) {
    int num1 = ( number - 108 ) / 256;
    int num2 = ( number - 108 ) % 256;

    put_int8( encoded, num1 + 247 );
    put_int8( encoded, num2 );
  }

  return encoded;
}

/* Decode a charstring number to an int, as defined in the Type 1 Font Format */
int32_t decode_charstring_number( BUFFER *number ) {
  int key = number->data[0];
  int num2 = ( number->size > 1 ) ? number->data[1] : 0;
  int num3 = ( number->size > 2 ) ? number->data[2] : 0;

  if ( key == 28 )
    return ( ( int16_t )( num2 * 256 + num3 ) );
  if ( key >= 32 && key <= 246 )
    return key - 139;
  if ( key >= 247 && key <= 250 )
    return ( ( key - 247 ) * 256 ) + num2 + 108;
  if ( key >= 251 && key <= 254 )
    return ( ( key - 251 ) * -256 ) - num2 - 108;

  return 0;
}

/* The offsets are where the blocks would have been if the top dict
 hadn't held their offsets - that is, if this function returned a 0-byte buffer.
 */
BUFFER *make_topdict_offsets( PRIVATE_DICT_OFFSETS *offsets ) {

  /* First, decide how many bytes this will take. */
  BUFFER *private_len_enc = encode_charstring_number( offsets->private_len );
  int guessed_len = 8 + private_len_enc->position; /* Absolute minimum */
  int calculated_len = guessed_len;
  do {
    guessed_len = calculated_len;
    calculated_len = 4 + private_len_enc->position; /* Operators */
    calculated_len +=
        get_encoded_charstring_len( offsets->charstrings_pos + guessed_len );
    calculated_len +=
        get_encoded_charstring_len( offsets->private_pos + guessed_len );
    calculated_len +=
        get_encoded_charstring_len( offsets->encoding_pos + guessed_len );
    calculated_len +=
        get_encoded_charstring_len( offsets->charset_pos + guessed_len );
  } while ( guessed_len != calculated_len );

  BUFFER *res = makebuffer( calculated_len );
  BUFFER *charstring_enc =
      encode_charstring_number( offsets->charstrings_pos + calculated_len );
  BUFFER *private_enc =
      encode_charstring_number( offsets->private_pos + calculated_len );
  BUFFER *encoding_enc =
      encode_charstring_number( offsets->encoding_pos + calculated_len );
  BUFFER *charset_enc =
      encode_charstring_number( offsets->charset_pos + calculated_len );

  insert_buffer( res, charstring_enc );
  put_int8( res, 17 );

  insert_buffer( res, private_len_enc );
  insert_buffer( res, private_enc );
  put_int8( res, 18 );

  insert_buffer( res, encoding_enc );
  put_int8( res, 16 );

  insert_buffer( res, charset_enc );
  put_int8( res, 15 );

  free_buffer( charstring_enc );
  free_buffer( private_enc );
  free_buffer( private_len_enc );
  free_buffer( encoding_enc );
  free_buffer( charset_enc );

  return res;
}

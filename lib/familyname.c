/*

  Fluxfonts – a continual font generator for increased privacy
  Copyright 2012–2017, Daniel Aleksandersen
  Copyright 2012, Daniel Nebdal
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

#include <stdint.h>

#if defined( _WIN32 ) || defined( _WIN64 )
typedef uint16_t wchar_t;
#endif

#include "familyname.h"

const char majusculeAlphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char minusculeAlphabet[] = "aaabcdeeefghiijklmnnooopqrstuuuvwxyz";

struct names *make_fontnames( ) {

  struct names *font_names = (struct names *) malloc( sizeof( struct names ) );

  /* Seed name generator with dictionary file if present in standard locations.
   */
  if ( dict_file == NULL )
    dict_file = mmap_file( "/usr/share/dict/words" );
  if ( dict_file == NULL )
    dict_file = mmap_file( "/usr/share/words" );

  if ( dict_file != NULL )
    dict_lines = count_lines( dict_file );

  char *foundryVendorID = random_majuscule_characters( 4 );

  const char *copyrightNotice = "All rights to this work have been "
                                "relinquished. The work is in the Public "
                                "Domain.";

  const char *fontFamilyStyle = "Regular";

  int familyNameWords = rand( ) % 3 + 1;
  int fontFamilyNamelen = WORDLEN_MAX * familyNameWords + 1;
  char *fontFamilyName = malloc( fontFamilyNamelen );
  int postscriptNamelen =
      WORDLEN_MAX * familyNameWords + 1 + strlen( fontFamilyStyle );
  char *postscriptName = malloc( postscriptNamelen );
  fontFamilyName[0] = '\0';
  postscriptName[0] = '\0';
  for ( int i = 0; i < familyNameWords; i++ ) {
    char *word = random_word( );
    strncat( fontFamilyName, word, fontFamilyNamelen );
    strncat( postscriptName, word, postscriptNamelen );
    free( word );
    if ( ( i + 1 ) < familyNameWords ) {
      strncat( fontFamilyName, " ", fontFamilyNamelen );
    }
  }
  strncat( postscriptName, fontFamilyStyle, postscriptNamelen );

  if ( dict_file != NULL )
    free_positions( dict_lines );

  char *versionString = "Version 1.000";

  int fullFontNamelen =
      strlen( fontFamilyName ) + strlen( fontFamilyStyle ) + 2;
  char *fullFontName = malloc( fullFontNamelen );
  fullFontName[0] = '\0';
  strncat( fullFontName, fontFamilyName, fullFontNamelen );
  strncat( fullFontName, " ", fullFontNamelen );
  strncat( fullFontName, fontFamilyStyle, fullFontNamelen );

  int uniqueFontNamelen = strlen( foundryVendorID ) + strlen( fullFontName ) +
                          strlen( versionString ) + 3;
  char *uniqueFontName = malloc( uniqueFontNamelen );
  uniqueFontName[0] = '\0';

  strncat( uniqueFontName, foundryVendorID, uniqueFontNamelen );
  strncat( uniqueFontName, ";", uniqueFontNamelen );
  strncat( uniqueFontName, fullFontName, uniqueFontNamelen );
  strncat( uniqueFontName, ";", uniqueFontNamelen );
  strncat( uniqueFontName, versionString, uniqueFontNamelen );

  font_names->copyrightNotice = copyrightNotice;
  font_names->fontFamilyName = fontFamilyName;
  font_names->fontFamilyStyle = fontFamilyStyle;
  font_names->uniqueFontName = uniqueFontName;
  font_names->versionString = versionString;
  font_names->fullFontName = fullFontName;
  font_names->postscriptName = postscriptName;
  font_names->foundryVendorID = foundryVendorID;

  return font_names;
}

char *random_majuscule_characters( int numberOfCharacters ) {

  int randchari;
  char *random_majuscule_characters = malloc( numberOfCharacters + 1 );

  for ( int i = 0; i < numberOfCharacters; i++ ) {
    randchari = rand( ) % ( sizeof( majusculeAlphabet ) - 1 );
    random_majuscule_characters[i] = majusculeAlphabet[randchari];
  }
  random_majuscule_characters[numberOfCharacters] = 0;

  return random_majuscule_characters;
}

char *random_minuscule_characters( int numberOfCharacters ) {

  int randchari;
  char *random_minuscule_characters = malloc( numberOfCharacters + 1 );

  for ( int i = 0; i < numberOfCharacters; i++ ) {
    randchari = rand( ) % ( sizeof( minusculeAlphabet ) - 1 );
    random_minuscule_characters[i] = minusculeAlphabet[randchari];
  }
  random_minuscule_characters[numberOfCharacters] = 0;

  return random_minuscule_characters;
}

BUFFER *mmap_file( char *filename ) {
#if defined( _WIN32 ) || defined( _WIN64 )
  return NULL;
#else
  int fd = open( filename, O_RDONLY );
  if ( fd == -1 )
    return NULL;

  BUFFER *result = malloc( sizeof( BUFFER ) );
  struct stat *fileInfo = malloc( sizeof( struct stat ) );
  fstat( fd, fileInfo );
  result->data = mmap( NULL, fileInfo->st_size, PROT_READ, MAP_PRIVATE, fd, 0 );
  result->size = fileInfo->st_size;
  result->position = 0;
  free( fileInfo );

  return result;
#endif
}

POSITIONS *count_lines( BUFFER *data ) {

  POSITIONS *lineBuffer = malloc( sizeof( POSITIONS ) );

  int lines = 0;
  int i = 0;
  while ( i < data->size ) {
    if ( data->data[i] == '\n' )
      lines++;
    i++;
  }

  lineBuffer->count = lines;
  lineBuffer->positions = malloc( sizeof( int ) * lines );
  lineBuffer->positions[0] = 0;
  int lineBufferPos = 1;

  for ( int i = 1; i < ( data->size - 1 ); i++ ) {
    if ( data->data[i] == '\n' )
      lineBuffer->positions[lineBufferPos++] = i + 1;
  }

  return lineBuffer;
}

char *word_from_pos( char *data ) {

  int length = 0;
  char *dataPointer = data;
  while ( isalnum( *dataPointer ) ) {
    length++;
    dataPointer++;
  }

  char *word = malloc( length + 1 );
  memcpy( word, data, length );
  word[length] = 0;

  return word;
}

char *random_word( ) {

  if ( dict_file == NULL ) {
    char *word = malloc( 10 );

    memset( word, 0, 10 );

    strncat( word, random_majuscule_characters( 1 ), 10 );
    strncat( word, random_minuscule_characters( rand( ) % 8 + 1 ), 10 );

    return word;

  } else {
    int wordNum = rand( ) % ( dict_lines->count + 1 );
    int wordOffset = dict_lines->positions[wordNum];
    char *wordStart = (char *) &dict_file->data[wordOffset];
    char *word = word_from_pos( wordStart );
    word[0] = toupper( word[0] );

    return word;
  }
}

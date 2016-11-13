/*

  Fluxfonts – a continual font generator for increased privacy
  Copyright 2012–2016, Daniel Aleksandersen
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


#include "familyname.h"


const char majusculeAlphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char minusculeAlphabet[] = "aaabcdeeefghiijklmnnooopqrstuuuvwxyz";


struct names *make_fontnames() {

  struct names *font_names = ( struct names* ) malloc( sizeof( struct names ) );

  /* Seed name generator with dictionary file if present in standard locations. */
  if ( dict_file == NULL )
    dict_file = mmap_file( "/usr/share/dict/words" );
  if ( dict_file == NULL )
    dict_file = mmap_file( "/usr/share/words" );

  if ( dict_file != NULL )
    dict_lines = count_lines( dict_file );


  char *foundryVendorID = random_majuscule_characters( 4 );

  const char *copyrightNotice = "All rights to this work have been relinquished. The work is in the Public Domain.";

  const char *fontFamilyStyle = "Regular";

  int familyNameWords = rand() % 3 + 1;
  char *fontFamilyName = malloc( WORDLEN_MAX * familyNameWords + 1 );
  char *postscriptName = malloc( WORDLEN_MAX * familyNameWords + 1 + strlen( fontFamilyStyle ) );
  fontFamilyName[0] = '\0';
  postscriptName[0] = '\0';
  for(int i=0; i<familyNameWords; i++) {
    char *word = random_word();
    strcat( fontFamilyName, word );
    strcat( postscriptName, word );
    free(word);
    if ( (i+1) < familyNameWords ) {
      strcat( fontFamilyName, " " );
    }
  }
  strcat( postscriptName, fontFamilyStyle );

  if ( dict_file != NULL )
    free_positions( dict_lines );

  char *versionString = "Version 1.000";
  char *fullFontName = malloc( strlen( fontFamilyName ) + strlen( fontFamilyStyle ) + 2 );
  fullFontName[0] = '\0';
  strcat( fullFontName, fontFamilyName );
  strcat( fullFontName, " ");
  strcat( fullFontName, fontFamilyStyle );

  char *uniqueFontName = malloc( strlen( foundryVendorID ) + strlen( fullFontName ) + strlen( versionString ) + 3 );
  uniqueFontName[0] = '\0';
  strcat( uniqueFontName, foundryVendorID );
  strcat( uniqueFontName, ";" );
  strcat( uniqueFontName, fullFontName );
  strcat( uniqueFontName, ";" );
  strcat( uniqueFontName, versionString );

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


char* random_majuscule_characters( int numberOfCharacters ) {

  char *random_majuscule_characters = malloc( numberOfCharacters + 1 );

  for ( int i = 0; i < numberOfCharacters; i++ ) {
    random_majuscule_characters[i] = majusculeAlphabet[rand() % ( sizeof( majusculeAlphabet ) - 1 )];
  }
  random_majuscule_characters[numberOfCharacters] = 0;

  return random_majuscule_characters;
}


char* random_minuscule_characters( int numberOfCharacters ) {

  char *random_minuscule_characters = malloc( numberOfCharacters + 1 );

  for ( int i = 0; i < numberOfCharacters; i++ ) {
    random_minuscule_characters[i] = minusculeAlphabet[rand() % ( sizeof( minusculeAlphabet ) - 1 )];
  }
  random_minuscule_characters[numberOfCharacters] = 0;

  return random_minuscule_characters;
}


BUFFER *mmap_file( char *filename ) {

  int fd = open( filename, O_RDONLY );
  if ( fd == -1 ) return NULL;

  BUFFER *result = malloc( sizeof( BUFFER ) );
  struct stat *fileInfo = malloc( sizeof( struct stat ) );
  fstat( fd, fileInfo );
  result->data = mmap( NULL, fileInfo->st_size, PROT_READ, MAP_PRIVATE, fd, 0 );
  result->size = fileInfo->st_size;
  result->position = 0;
  free( fileInfo );

  return result;
}


POSITIONS *count_lines( BUFFER *data ) {

  POSITIONS *lineBuffer = malloc( sizeof( POSITIONS ) );

  int lines = 0;
  for( int i=0; i < data->size; i++ ) {
    if ( data->data[i] == '\n' ) lines++;
  }

  lineBuffer->count = lines;
  lineBuffer->positions = malloc( sizeof( int ) * lines );
  lineBuffer->positions[0] = 0;
  int lineBufferPos = 1;

  for( int i=1; i < ( data->size - 1 ); i++ ) {
    if ( data->data[i] == '\n' )
      lineBuffer->positions[lineBufferPos++] = i+1;
  }

  return lineBuffer;
}


char *word_from_pos( char* data ) {

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

char *random_word() {

  if ( dict_file == NULL ) {
    char *word = malloc( 10 );
    bzero( word, 10) ;
    strcat( word, random_majuscule_characters( 1 ) );
    strcat( word, random_minuscule_characters( rand() % 8 + 1 ) );

    return word;

  } else {
    int wordNum = rand() % ( dict_lines->count + 1 );
    int wordOffset = dict_lines->positions[wordNum];
    char *wordStart = ( char* ) &dict_file->data[wordOffset];
    char *word = word_from_pos( wordStart );
    word[0] = toupper( word[0] );

    return word;
  }
}

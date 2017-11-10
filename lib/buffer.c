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

#include "buffer.h"

BUFFER *makebuffer( int size ) {

  BUFFER *result = malloc( sizeof( BUFFER ) );
  result->size = size;
  result->position = 0;
  result->data = (uint8_t *) malloc( result->size );
  memset( result->data, 0, size );

  return result;
}

void *buffer_alloc( BUFFER *buf, int size ) {

  void *result;

  if ( buf->size - buf->position < size ) {
    exit( ENOMEM );
  }

  result = &buf->data[buf->position];
  buf->position += size;

  return result;
}

void insert_buffer( BUFFER *target, BUFFER *source ) {

  void *dst = buffer_alloc( target, source->position );

  memcpy( dst, source->data, source->position );
}

void *get_current( BUFFER *buf ) { return &buf->data[buf->position]; }

size_t get_offset( BUFFER *buf, void *ptr ) {

  return (size_t) ptr - (size_t) buf->data;
}

uint8_t *put_int8( BUFFER *buf, uint8_t value ) {

  uint8_t *result = buffer_alloc( buf, 1 );
  *result = value;
  return result;
}

uint16_t *put_int16( BUFFER *buf, uint16_t value ) {

  uint16_t *result = buffer_alloc( buf, 2 );
  *result = value;
  return result;
}

void free_buffer( BUFFER *buf ) {

  free( buf->data );
  free( buf );
}

void free_positions( POSITIONS *pos ) {

  free( pos->positions );
  free( pos );
}

void *copy_string_to_buffer( BUFFER *buf, char *string ) {

  int length = strlen( string );
  char *destination = buffer_alloc( buf, length );
  memcpy( destination, string, length );

  return destination;
}

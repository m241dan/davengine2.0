/* Davenge String for the Davengine and other General Use */
/* written by Davenge */

#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include "dstring.h"

/* statics */
/* deconstructors */
static void free_dstring( DSTRING *string );
static void free_raw_string( RAW_DSTRING *raw_string );

/* actions */
static void toss_into_can( DSTRING *string );
static void takeout_string( DSTRING *trash );
static void attach_string( DSTRING *string );
static void detach_string( DSTRING *string );

/* checkers */
static int check_bucket_for_dstring( DSTRING *string );
/* end statics */

/* global can */

GARBAGE_CAN *string_garbage_can;

/* constructors */

RAW_DSTRING *new_raw_string( const char *string )
{
   RAW_DSTRING *raw_string;
   int x, length = strlen( string );
   size_t size = length > 100 ? ( length * 2 ) : ( length + 100 ); /* gives us wiggle room to adjust the string without reallocing */


   raw_string = (RAW_DSTRING *)calloc( 1 , sizeof( RAW_DSTRING ) );
   raw_string->data = (char *)calloc( size, sizeof( char ) );

   for( x = 0; string[x] != '\0'; x++ )
      raw_string->data[x] = string[x];
   raw_string->data[x] = '\0';
   raw_string->size = size;
   raw_string->length = length;
   return raw_string;
}

DSTRING *new_string( const char *fmt, ... )
{
   DSTRING *string;
   char *new_string;
   va_list va;
   int length;

   string = (DSTRING *)calloc( 1, sizeof( DSTRING ) );
   string->next_gc = NULL;
   string->position = -1;
   string->a_size = -1;

   if( !fmt || fmt[0] == '\0' )
   {
      string->raw_data = new_raw_string( "nil" );
      string->raw_data->reach++;
      update_collection( string, DEFAULT_LIFE );
      return string;
   }

   /* we need to find the length of the strength for allocating after format, which is tricky.
    * to solve the trickiness, we make a small char pointer of 1 byte and try to shove the
    * formatted string into it. vsnprintf will return the actual length we need with the format
    * included. Then, we just do it again with the proper length */

   new_string = (char *)calloc( 1, sizeof( char ) );
   va_start( va, fmt );
   length = vsnprintf( new_string, 1, fmt, va );
   va_end( va );
   free( new_string );

   new_string = (char *)calloc( ( length + 1 ), sizeof( char ) );
   va_start( va, fmt );
   length = vsnprintf( new_string, ( length + 1 ), fmt, va );
   va_end( va );

   string->raw_data = new_raw_string( new_string );
   string->raw_data->reach++;
   free( new_string ); /* new_raw_string makes it's own copy */

   update_collection( string, DEFAULT_LIFE );
   return string;
}

DSTRING *new_string_nogc( const char *fmt, ... )
{
   DSTRING *string;
   char *new_string;
   va_list va;
   int length;

   string = (DSTRING *)calloc( 1, sizeof( DSTRING ) );
   string->next_gc = NULL;
   string->position = -1;
   string->a_size = -1;
   string->life = DONT_COLLECT;

   if( !fmt || fmt[0] == '\0' )
   {
      string->raw_data = new_raw_string( "nil" );
      string->raw_data->reach++;
      return string;
   }

   /* we need to find the length of the strength for allocating after format, which is tricky.
    * to solve the trickiness, we make a small char pointer of 1 byte and try to shove the
    * formatted string into it. vsnprintf will return the actual length we need with the format
    * included. Then, we just do it again with the proper length */

   new_string = (char *)calloc( 1, sizeof( char ) );
   va_start( va, fmt );
   length = vsnprintf( new_string, 1, fmt, va );
   va_end( va );
   free( new_string );

   new_string = (char *)calloc( ( length + 1 ), sizeof( char ) );
   va_start( va, fmt );
   length = vsnprintf( new_string, ( length + 1 ), fmt, va );
   va_end( va );

   string->raw_data = new_raw_string( new_string );
   string->raw_data->reach++;
   free( new_string ); /* new_raw_string makes it's own copy */
   return string;
}

GARBAGE_CAN *setup_garbage_can( void )
{
   GARBAGE_CAN *can;

   can = (GARBAGE_CAN *)calloc( 1, sizeof( GARBAGE_CAN ) );
   can->collection_bucket = NULL;
   return can;
}

/* deconstructors */
static void free_dstring( DSTRING *string )
{
   string->next_gc = NULL; /* this should be handled by the takeout_string method */
   if( string->array )
      string->array[string->position] = NULL;
   if( --string->raw_data->reach == 0 )
      free_raw_string( string->raw_data );
   string->raw_data = NULL;
   free( string );
}

static void free_raw_string( RAW_DSTRING *raw_string )
{
   free( raw_string->data );
   free( raw_string );
}

void takeout_can( void )
{
   GARBAGE_CAN *can = string_garbage_can;
   DSTRING *string, *string_next;
   for( string = can->collection_bucket; string; string = string_next )
   {
      string_next = string->next_gc;
      free_dstring( string );
   }
   can->collection_bucket = NULL;
   free( can );
}

/* actions */
static void toss_into_can( DSTRING *string )
{
   if( string->life == DONT_COLLECT ) /* don't toss something that's not collected into the can */
      return;
   if( string->next_gc ) /* don't double add strings, will cause garbage collection to crash */
      return;
   if( check_bucket_for_dstring( string ) ) /* again, want to avoid any doubles */
      return;
   attach_string( string );
}

static void takeout_string( DSTRING *trash )
{
   detach_string( trash );
   free_dstring( trash );
}

static void attach_string( DSTRING *string )
{
   string->next_gc = string_garbage_can->collection_bucket;
   string_garbage_can->collection_bucket = string;
   string_garbage_can->count++;
}

static void detach_string( DSTRING *string )
{
   DSTRING *bucket = string_garbage_can->collection_bucket;
   if( bucket == string )
   {
      string_garbage_can->collection_bucket = string->next_gc;
      string->next_gc = NULL;
      string_garbage_can->count--;
      return;
   }
   while( bucket )
   {
      if( bucket->next_gc == string )
      {
         bucket->next_gc = string->next_gc;
         string->next_gc = NULL;
         string_garbage_can->count--;
         break;
      }
      bucket = bucket->next_gc;
   }
}

/* checkers */
static int check_bucket_for_dstring( DSTRING *string )
{
   DSTRING *bucket = string_garbage_can->collection_bucket;
   while( bucket )
   {
      if( bucket == string )
         return 1;
      bucket = bucket->next_gc;
   }
   return 0;
}

/* the trash man! */
void trash_man( void )
{
   DSTRING *string, *string_next;

   if( ( string = string_garbage_can->collection_bucket ) == NULL )
      return;

   while( string )
   {
      string_next = string->next_gc;
      update_collection( string, ( string->life - 1 ) );
      string = string_next;
   }
}

/* utility */
void update_collection( DSTRING *string, int life )
{
   int in_the_can = check_bucket_for_dstring( string );
   if( life == DONT_COLLECT && in_the_can )
      detach_string( string );
   else if( life <= 0 && life != DONT_COLLECT )
   {
      if( in_the_can )
         takeout_string( string );
      else
         free_dstring( string );
      return;
   }
   else if( life > 0 && !in_the_can )
   {
      string->life = life;
      toss_into_can( string );
      return;
   }
   string->life = life;
}

void inspect_can( void )
{
   GARBAGE_CAN *can = string_garbage_can;
   DSTRING *string;

   printf( "%s: There are %d strings in the can.\r\n", __FUNCTION__, can->count );
   if( ( string = can->collection_bucket ) == NULL )
      return;
   while( string )
   {
      printf( "%s: string(%d) \"%s\"\r\n", __FUNCTION__, string->life, rawstr( string ) );
      string = string->next_gc;
   }
}

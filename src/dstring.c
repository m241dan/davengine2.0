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
   raw_string->data[x] 	= '\0';
   raw_string->size 	= size;
   raw_string->length 	= length;
   return raw_string;
}

DSTRING *new_string( const char *fmt, ... )
{
   DSTRING *string;
   char *new_string;
   va_list va;
   int length;

   string 		= (DSTRING *)calloc( 1, sizeof( DSTRING ) );
   string->next_gc 	= NULL;
   string->position 	= -1;
   string->a_size 	= -1;
   string->life		= DEFAULT_LIFE;

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

   string		= (DSTRING *)calloc( 1, sizeof( DSTRING ) );
   string->next_gc 	= NULL;
   string->position 	= -1;
   string->a_size 	= -1;
   string->life 	= DONT_COLLECT;

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

int compare( DSTRING *compare_to, DSTRING *string )
{
   int len1 = length( compare_to );
   int len2 = length( string );

   if( len1 != len2 )
      return 0;
   if( str( compare_to ) == str( string ) )
      return 1;
   for( int x = 0; x < len1; x++ )
      if( compare_to->raw_data->data[x+compare_to->cursor] != string->raw_data->data[x+string->cursor] )
         return 0;
   return 1;
}

int raw_compare( DSTRING *compare_to, DSTRING *string )
{
   int len1 = rawlength( compare_to );
   int len2 = rawlength( string );

   if( len1 != len2 )
      return 0;
   if( rawstr( compare_to ) == rawstr( string ) )
      return 1;
   for( int x = 0; x < len1; x++ )
      if( compare_to->raw_data->data[x] != string->raw_data->data[x] )
         return 0;
   return 1;
}

int compare1( DSTRING *compare_to, const char *string )
{
   int len1 = length( compare_to );
   int len2 = strlen( string );

   if( len1 != len2 )
      return 0;
   if( str( compare_to ) == string )
      return 1;
   for( int x = 0; x < len1; x++ )
      if( compare_to->raw_data->data[x+compare_to->cursor] != *(string+x) )
         return 0;
   return 1;
}

int raw_compare1( DSTRING *compare_to, const char *string )
{
   int len1 = rawlength( compare_to );
   int len2 = strlen( string );

   if( len1 != len2 )
      return 0;
   if( rawstr( compare_to ) == string )
      return 1;
   for( int x = 0; x < len1; x++ )
      if( compare_to->raw_data->data[x] != *(string+x) )
         return 0;
   return 1;
}

int compare_nocase( DSTRING *compare_to, DSTRING *string )
{
   int len1 = length( compare_to );
   int len2 = length( string );

   if( len1 != len2 )
      return 0;
   if( str( compare_to ) == str( string ) )
      return 1;
   for( int x = 0; x < len1; x++ )
      if( tolower( compare_to->raw_data->data[x+compare_to->cursor] ) != tolower( string->raw_data->data[x+string->cursor] ) )
         return 0;
   return 1;
}

int raw_compare_nocase( DSTRING *compare_to, DSTRING *string )
{
   int len1 = rawlength( compare_to );
   int len2 = rawlength( string );

   if( len1 != len2 )
      return 0;
   if( rawstr( compare_to ) == rawstr( string ) )
      return 1;
   for( int x = 0; x < len1; x++ )
      if( tolower( compare_to->raw_data->data[x] ) != tolower( string->raw_data->data[x] ) )
         return 0;
   return 1;

}

int compare_nocase1( DSTRING *compare_to, const char *string )
{
   int len1 = length( compare_to );
   int len2 = strlen( string );

   if( len1 != len2 )
      return 0;
   if( str( compare_to ) == string )
      return 1;
   for( int x = 0; x < len1; x++ )
      if( tolower( compare_to->raw_data->data[x+compare_to->cursor] ) != tolower( *(string+x) ) )
         return 0;
   return 1;

}

int raw_compare_nocase1( DSTRING *compare_to, const char *string )
{
   int len1 = rawlength( compare_to );
   int len2 = strlen( string );

   if( len1 != len2 )
      return 0;
   if( rawstr( compare_to ) == string )
      return 1;
   for( int x = 0; x < len1; x++ )
      if( tolower( compare_to->raw_data->data[x] ) != tolower( *(string+x) ) )
         return 0;
   return 1;
}

int compare_prefix( DSTRING *compare_to, DSTRING *string )
{
   int len1 = length( compare_to );
   int len2 = length( string );

   if( len2 > len1 )
      return 0;
   if( str( compare_to ) == str( string ) )
      return 1;
   for( int x = 0; x < len2; x++ )
      if( compare_to->raw_data->data[x+compare_to->cursor] != string->raw_data->data[x+string->cursor] )
         return 0;
   return 1;
}

int raw_compare_prefix( DSTRING *compare_to, DSTRING *string )
{
   int len1 = rawlength( compare_to );
   int len2 = rawlength( string );

   if( len2 > len1 )
      return 0;
   if( rawstr( compare_to ) == rawstr( string ) )
      return 1;
   for( int x = 0; x < len2; x++ )
      if( compare_to->raw_data->data[x] != string->raw_data->data[x] )
         return 0;
   return 1;

}

int compare_prefix1( DSTRING *compare_to, const char *string )
{
   int len1 = length( compare_to );
   int len2 = strlen( string );

   if( len2 > len1 )
      return 0;
   if( str( compare_to ) == string )
      return 1;
   for( int x = 0; x < len2; x++ )
      if( compare_to->raw_data->data[x+compare_to->cursor] != *(string+x) )
         return 0;
   return 1;
}

int raw_compare_prefix1( DSTRING *compare_to, const char *string )
{
   int len1 = rawlength( compare_to );
   int len2 = strlen( string );

   if( len2 > len1 )
      return 0;
   if( rawstr( compare_to ) == string )
      return 1;
   for( int x = 0; x < len2; x++ )
      if( compare_to->raw_data->data[x] != *(string+x) )
         return 0;
   return 1;

}

DSTRING *softcopy( DSTRING *string )
{
   DSTRING *copy;

   if( !string || !string->raw_data )
   {
      printf( "%s: could not make soft copy of NULL, returning a \"nil\" string.\r\n", __FUNCTION__ );
      return new_string( NULL );
   }
   copy 		= (DSTRING *)calloc( 1, sizeof( DSTRING ) );
   copy->raw_data 	= string->raw_data;
   copy->cursor 	= string->cursor;
   copy->raw_data->reach++;
   update_collection( copy, DEFAULT_LIFE );
   return copy;
}

DSTRING *softcopy_nogc( DSTRING *string )
{
   DSTRING *copy;

   if( !string || !string->raw_data )
   {
      printf( "%s: could not make soft copy of NULL, returning a \"nil\" string.\r\n", __FUNCTION__ );
      return new_string_nogc( NULL );
   }
   copy 		= (DSTRING *)calloc( 1, sizeof( DSTRING ) );
   copy->raw_data       = string->raw_data;
   copy->cursor         = string->cursor;
   copy->raw_data->reach++;
   update_collection( copy, DONT_COLLECT );
   return copy;
}

DSTRING *softcopy_lit( DSTRING *string )
{
   DSTRING *copy;

   if( !string || !string->raw_data )
   {
      printf( "%s: could not make soft copy of NULL, returning a \"nil\" string.\r\n", __FUNCTION__ );
      return new_string_nogc( NULL );
   }
   copy 		= (DSTRING *)calloc( 1, sizeof( DSTRING ) );
   copy->raw_data       = string->raw_data;
   copy->cursor         = string->cursor;
   copy->array          = string->array;
   copy->position       = string->position;
   copy->a_size         = string->a_size;
   copy->raw_data->reach++;
   update_collection( copy, string->life );
   return copy;

}

DSTRING *hardcopy( DSTRING *string )
{
   DSTRING *copy;

   if( !string || !string->raw_data )
   {
      printf( "%s: could not make hard copy of NULL, returning a \"nil\" string.\r\n", __FUNCTION__ );
      return new_string( NULL );
   }
   copy 			= (DSTRING *)calloc( 1, sizeof( DSTRING ) );
   copy->raw_data 		= (RAW_DSTRING *)calloc( 1, sizeof( RAW_DSTRING ) );
   copy->raw_data->data 	= (char *)calloc( string->raw_data->size, sizeof( char ) );
   copy->raw_data->size 	= string->raw_data->size;
   copy->raw_data->length 	= string->raw_data->length;
   copy->raw_data->reach++;
   memcpy( copy->raw_data->data, string->raw_data->data, string->raw_data->size );
   update_collection( copy, DEFAULT_LIFE );
   return copy;
}

DSTRING *hardcopy_nogc( DSTRING *string )
{
   DSTRING *copy;

   if( !string || !string->raw_data )
   {
      printf( "%s: could not make hard copy of NULL, returning a \"nil\" string.\r\n", __FUNCTION__ );
      return new_string_nogc( NULL );
   }
   copy                         = (DSTRING *)calloc( 1, sizeof( DSTRING ) );
   copy->raw_data               = (RAW_DSTRING *)calloc( 1, sizeof( RAW_DSTRING ) );
   copy->raw_data->data         = (char *)calloc( string->raw_data->size, sizeof( char ) );
   copy->raw_data->size         = string->raw_data->size;
   copy->raw_data->length       = string->raw_data->length;
   copy->raw_data->reach++;
   memcpy( copy->raw_data->data, string->raw_data->data, string->raw_data->size );
   update_collection( copy, DONT_COLLECT );
   return copy;
}

DSTRING *downcase( DSTRING *string )
{
   DSTRING *downcase_string;

   if( !string || !string->raw_data )
   {
      printf( "%s: could not downcase a NULL, returning a \"ni;\" string.\r\n", __FUNCTION__ );
      return new_string( NULL );
   }
   downcase_string = hardcopy( string );
   for( int x = 0; x < rawlength( downcase_string ); x++ )
      downcase_string->raw_data->data[x] = tolower( downcase_string->raw_data->data[x] );
   return downcase_string;
}

int downcase1( DSTRING *string )
{
   if( !string || !string->raw_data )
   {
      printf( "%s: passed a NULL string.\r\n", __FUNCTION__ );
      return 0;
   }
   for( int x = 0; x < rawlength( string ); x++ )
      string->raw_data->data[x] = tolower( string->raw_data->data[x] );
   return 1;
}

DSTRING *uppercase( DSTRING *string )
{
   DSTRING *uppercase_string;
   if( !string || !string->raw_data )
   {
      printf( "%s: passed a NULL string, returning a \"nil\".\r\n", __FUNCTION__ );
      return new_string( NULL );
   }
   uppercase_string = hardcopy( string );
   for( int x = 0; x < rawlength( uppercase_string ); x++ )
      uppercase_string->raw_data->data[x] = toupper( uppercase_string->raw_data->data[x] );
   return uppercase_string;
}

int uppercase1( DSTRING *string )
{
   if( !string || !string->raw_data )
   {
      printf( "%s: passed a NULL string.\r\n", __FUNCTION__ );
      return 0;
   }
   for( int x = 0; x < rawlength( string ); x++ )
      string->raw_data->data[x] = toupper( string->raw_data->data[x] );
   return 1;
}

DSTRING *capitalize( DSTRING *string )
{
   DSTRING *capitalized_string;
   int cap_next, x = 0;
   if( !string || !string->raw_data )
   {
      printf( "%s: passed a NULL string, returning a \"nil\" string.\r\n", __FUNCTION__ );
      return new_string( NULL );
   }
   capitalized_string = downcase( string );
   while( isspace( rawstr( capitalized_string )[x] ) )
      x++;
   rawstr(capitalized_string)[x] = toupper( rawstr( capitalized_string )[x] );

   for( ; x < rawlength( capitalized_string ); x++ )
   {
      if( isspace( rawstr( capitalized_string )[x] ) )
         cap_next = 1;
      else if( cap_next )
      {
         rawstr(capitalized_string)[x] = toupper( rawstr( capitalized_string )[x] );
         cap_next = 0;
      }
      continue;
   }
   return capitalized_string;
}

int capitalize1( DSTRING *string )
{
   int cap_next, x = 0;
   if( !string || !string->raw_data )
   {
      printf( "%s: passed a NULL string.\r\n", __FUNCTION__ );
      return 0;
   }
   downcase1( string );
   while( isspace( rawstr( string )[x] ) )
      x++;
   rawstr( string )[x] = toupper( rawstr( string )[x] );

   for( x = (x+1); x < rawlength( string ); x++ )
   {
      if( isspace( rawstr( string )[x] ) )
         cap_next = 1;
      else if( cap_next )
      {
         rawstr( string )[x] = toupper( rawstr( string )[x] );
         cap_next = 0;
      }
      continue;
   }
   return 1;
}

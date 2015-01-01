/* Davenge String for the Davengine and other General Use */
/* written by Davenge */

#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>

/* constructors */

RAW_DSTRING *new_raw_string( const char *string )
{
   RAW_DSTRING *raw_string;
   int length = strlen( string );
   size_t size = length > 100 ? ( length * 2 ) : ( length + 100 ); /* gives us wiggle room to adjust the string without reallocing */


   raw_string = (RAW_DSTRING *)calloc( 1 , sizeof( RAW_DSTRING ) );
   raw_string->data = (char *)calloc( size, sizeof( char ) );

   while( *string != '\0' )
      *(raw_string->data)++ = *string++;
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
   string->position = -1;
   string->a_size = -1;
   string->life = 1;

   if( !fmt || fmt[0] == '\0' )
   {
      string->raw_data = new_raw_string( "nil" );
      string->raw_data->reach++;
      set_for_collection( string, 10 ); /* 10 seems safe, cpu pulses */
      return string;
   }

   /* we need to find the length of the strength for allocating after format, which is tricky.
    * to solve the trickiness, we make a small char pointer of 1 byte and try to shove the
    * formatted string into it. vsnprintf will return the actual length we need with the format
    * included. Then, we just do it again with the proper length */

   new_string = (char *)calloc( 1, sizeof( char ) );
   va_start( va, fmt );
   length = vsnprintf( new_string, 2, fmt, va );
   va_end( va );
   free( string );

   new_string = (char *)calloc( ( length + 1 ), sizeof( char ) );
   va_start( va, fmt );
   length = vsnprintf( new_string, ( length + 1 ), fmt, va );
   va_end( va );

   string->raw_data = new_raw_string( new_string );
   string->raw_data->reach++;
   free( new_string ); /* new_raw_string makes it's own copy */

   set_for_collection( string, 10 ); /* again, seems safe */
   return string;
}

GARBAGE_CAN *setup_garbage_can( void )
{
   GARBAGE_CAN *can;

   can = (GARBAGE_CAN *)calloc( 1, sizeof( GARBAGE_CAN ) );
   return can;
}

/* deconstructors */
void free_dstring( DSTRING *string )
{
   string->next_gc = NULL; /* this should be handled by the takeout_string method */
   string->array[position] = NULL;
   if( --string->raw_data->reach == 0 )
      free_raw_string( string->raw_data );
   string->raw_data = NULL;
   free( string );
}

void free_raw_string( RAW_STRING *raw_string )
{
   free( raw_string->data );
   free( raw_string );
}

void takeout_can( GARBAGE_CAN *can )
{
   DSTRING *string, string_next;
   for( string = can->collection_bucket; string; string = string_next )
   {
      string_next = string->next_gc;
      free_dstring( string );
   }
   for( string = can->no_collection_bucket; string; string = string_next )
   {
      string_next = string->next_gc;
      free_dstring( string );
   }

   can->collection_bucket = NULL;
   can->no_collection_bucket = NULL;
   free( can );
}

/* setters */

void set_for_collection( DSTRING *string, int life )
{
   string->life = life;
   toss_into_can( string );
}

/* actions */
void toss_into_can( DSTRING *string )
{
   DSTRING **bucket = string->life == DONT_COLLECT ? &(string_garbage_can->no_collection_bucket) : &(string_garbage_can->collection_bucket);
   int *count = string->life == DONT_COLLECT ? &string_garbage_can->no_collection_count : &string_garbage_can->collection_count;

   if( string->next_gc ) /* don't double add strings, will cause garbage collection to crash */
      return;
   if( check_bucket_for_dstring( *bucket, string ) ) /* again, want to avoid any doubles */
      return;

   string->next_gc = *bucket;
   *bucket = string;
   *count += 1;
}

void update_collection( DSTRING *string, int life )
{
   if( life <= 0 && life != DONT_COLLECT ) /* collect now */
      takeout_string( string );
   else
      string->life = life;
}

void takeout_string( DSTRING *trash )
{
   DSTRING **bucket = trash->life == DONT_COLLECT ? 
   DSTRING *string;
  

}

/* checkers */
int check_bucket_for_dstring( DSTRING *bucket, DSTRING *string )
{
   while( bucket )
   {
      if( bucket == string )
         return 1;
      bucket = bucket->next_gc;
   }
   return 0;
}

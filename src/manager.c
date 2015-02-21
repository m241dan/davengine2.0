/* the memory manager written by Davenge using the D_HASH and DLIST *
 * D_HASH written by Davenge DLIST expanded on an existing library  */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "list.h"
#include "buffers.h"
#include "hash.h"
#include "manager.h"


MEM_MANAGER *memory_management = NULL;

/* creators */

int init_manager( void )
{
   memory_management 			= malloc( sizeof( MEM_MANAGER ) );
   memory_management->zero_reach_list 	= AllocList();
   memory_management->reach_list 	= init_hash( NUMERIC_HASH, MANAGER_SIZE );
   return 1;
}

int new_bucket( MB_TYPE type, void *memory )
{
   MEM_BUCKET *bucket;

   if( !memory_management )
   {
      printf( "%s: memory manager not initiliazed.\n", __FUNCTION__ );
      return 0;
   }
   if( !memory )
   {
      printf( "%s: passed NULL memory to save. Returning NULL.\n", __FUNCTION__ );
      return 0;
   }
   bucket 		= malloc( sizeof( MEM_BUCKET ) );
   bucket->memory	= memory;
   bucket->type		= type;
   bucket->reach	= 0;
   AttachToList( bucket, memory_management->zero_reach_list );
   return 1;
}

int *new_integer( int num )
{
   int *ptr = malloc( sizeof( int ) );
   *ptr = num;
   new_bucket( MEM_INTEGER, ptr );
   return ptr;
}

char *new_string( const char *fmt, ... )
{
   char *ptr;
   va_list va;
   int length;

   if( !fmt || fmt[0] == '\0' )
   {
      ptr = malloc( sizeof( char ) * 4 );
      ptr[0] = 'n';
      ptr[1] = 'i';
      ptr[2] = 'l';
      ptr[3] = '\0';
      new_bucket( MEM_STRING, ptr );
      return ptr;
   }

   ptr = malloc( sizeof( char ) );
   va_start( va, fmt );
   length = vsnprintf( ptr, 2, fmt, va );
   va_end( va );
   free( ptr );

   ptr = malloc( sizeof( char ) * ( length + 1 ) );
   va_start( va, fmt );
   length = vsnprintf( ptr, ( length + 1 ), fmt, va );
   va_end( va );

   new_bucket( MEM_STRING, ptr );
   return ptr;
}

D_BUFFER *new_buffer( int width )
{
   D_BUFFER *buf = malloc( sizeof( D_BUFFER ) );
   buf->width = width;
   buf->favor = BOT_FAVOR;
   buf->lines = AllocList();

   new_bucket( MEM_BUFFER, buf );
   return buf;
}

/* destroyers */
int free_bucket( MEM_BUCKET *bucket )
{
   if( !bucket )
   {
      printf( "%s: attempt to free NULL bucket.\n", __FUNCTION__ );
      return 0;
   }

   DetachFromList( bucket, memory_management->zero_reach_list );

   if( bucket->reach > 0 )
   {
      printf( "%s: will not free bucket, still in reach somewhere.\n", __FUNCTION__ );
      /* put it into the hash */
      return 0;
   }

   switch( bucket->type )
   {
      case MEM_INTEGER:
      case MEM_STRING:
         free( bucket->memory );
         bucket->memory = NULL;
         break;
      case MEM_BUFFER:
         free_buffer( (D_BUFFER *)bucket->memory );
         bucket->memory = NULL;
         break;
   }
   free( bucket );
   return 1;
}

int free_buffer( D_BUFFER *buf )
{
   FreeList( buf->lines );
   free( buf );
   return 0;
}

/* getters */
int get_zero_reach_size( void )
{
   return SizeOfList( memory_management->zero_reach_list );
}

MEM_BUCKET *get_bucket_for( void *ptr )
{
   HASH_BUCKET *h_bucket;
   MEM_BUCKET *bucket;
   ITERATOR Iter;

   AttachIterator( &Iter, memory_management->zero_reach_list );
   while( ( bucket = (MEM_BUCKET *)NextInList( &Iter ) ) != NULL )
      if( bucket->memory == ptr )
         break;
   DetachIterator( &Iter );

   /* keyes are unique memory addresses so there is no need to check the data */
   if( !bucket )
      if( ( h_bucket = hash_find( memory_management->reach_list, (long)ptr ) ) != NULL )
         bucket = (MEM_BUCKET *)h_bucket->data;
   return bucket;
}

/* utility */
void reach_ptr( void *ptr )
{
   MEM_BUCKET *bucket;

   if( !ptr )
      return;
   if( ( bucket = get_bucket_for( ptr ) ) == NULL )
      return;
   if( ++bucket->reach == 1 )
   {
      DetachFromList( bucket, memory_management->zero_reach_list );
      hash_add( memory_management->reach_list, bucket, (long)ptr );
   }
   return;
}

void unreach_ptr( void *ptr )
{
   MEM_BUCKET *bucket;

   if( !ptr )
      return;
   if( ( bucket = get_bucket_for( ptr ) ) == NULL )
      return;
   if( --bucket->reach == 0 )
   {
      hash_remove( memory_management->reach_list, bucket, (long)ptr );
      AttachToList( bucket, memory_management->zero_reach_list );
   }
   return;
}

/* monitor */
void clear_zero_reach( void )
{
   MEM_BUCKET *bucket;
   ITERATOR Iter;

   AttachIterator( &Iter, memory_management->zero_reach_list );
   while( ( bucket = (MEM_BUCKET *)NextInList( &Iter ) ) != NULL )
      free_bucket( bucket );
   DetachIterator( &Iter );
}

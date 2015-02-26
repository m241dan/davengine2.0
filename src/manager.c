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

int new_bucket( MB_TYPE type, void *memory, size_t size )
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
   bucket->mem_size	= size;
   bucket->type		= type;
   bucket->reach	= AllocList();
   AttachToList( bucket, memory_management->zero_reach_list );
   return 1;
}

int *new_integer( int num )
{
   size_t size = sizeof( int );
   int *ptr = malloc( size );
   *ptr = num;
   new_bucket( MEM_INTEGER, ptr, size );
   return ptr;
}

char *new_string( const char *fmt, ... )
{
   char *ptr;
   va_list va;
   size_t size;
   int length;

   if( !fmt || fmt[0] == '\0' )
   {
      ptr = malloc( sizeof( char ) * 4 );
      ptr[0] = 'n';
      ptr[1] = 'i';
      ptr[2] = 'l';
      ptr[3] = '\0';
      new_bucket( MEM_STRING, ptr, (size_t)4 );
      return ptr;
   }

   ptr = malloc( sizeof( char ) );
   va_start( va, fmt );
   length = vsnprintf( ptr, 2, fmt, va );
   va_end( va );
   free( ptr );

   /* plus 1 for NULL terminator */
   size = length + 101;

   ptr = malloc( sizeof( char ) * ( length + 1 ) );
   va_start( va, fmt );
   length = vsnprintf( ptr, size , fmt, va );
   va_end( va );

   new_bucket( MEM_STRING, ptr, size );
   return ptr;
}

char *str_alloc( size_t size )
{
   char *ptr = calloc( sizeof( char ), size );
   ptr[0] = '\0';
   new_bucket( MEM_STRING, ptr, size );
   return ptr;
}

D_BUFFER *new_buffer( int width )
{
   size_t size = sizeof( D_BUFFER );
   D_BUFFER *buf = malloc( size );
   buf->width = width;
   buf->favor = BOT_FAVOR;
   buf->lines = AllocList();

   new_bucket( MEM_BUFFER, buf, size );
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

   if( SizeOfList( bucket->reach ) > 0 )
   {
      printf( "%s: will not free bucket, still in reach somewhere.\n", __FUNCTION__ );
      /* put it into the hash */
      return 0;
   }
   FreeList( bucket->reach );

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

MEM_BUCKET *get_bucket_for( const void *ptr )
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

size_t get_size( const void *ptr )
{
   MEM_BUCKET *bucket;

   if( ( bucket = get_bucket_for( ptr ) ) == NULL )
      return 0;
   return bucket->mem_size;
}

/* utility */
void reach_ptr( const void *ptr, void **assignment )
{
   MEM_BUCKET *bucket;

   if( !ptr )
      return;
   if( ( bucket = get_bucket_for( ptr ) ) == NULL )
      return;
   if( SizeOfList( bucket->reach ) == 0 )
   {
      DetachFromList( bucket, memory_management->zero_reach_list );
      hash_add( memory_management->reach_list, bucket, (long)ptr );
   }
   AttachToList( assignment, bucket->reach );
   hash_show( memory_management->reach_list );
   return;
}

void unreach_ptr( const void *ptr, void **assignment )
{
   MEM_BUCKET *bucket;

   if( !ptr )
      return;
   if( ( bucket = get_bucket_for( ptr ) ) == NULL )
      return;
   if( SizeOfList( bucket->reach ) == 1 )
   {
      hash_remove( memory_management->reach_list, bucket, (long)ptr );
      AttachToList( bucket, memory_management->zero_reach_list );
   }
   DetachFromList( assignment, bucket->reach );
   hash_show( memory_management->reach_list );
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

/* the memory manager written by Davenge using the D_HASH and DLIST *
 * D_HASH written by Davenge DLIST expanded on an existing library  */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "list.h"
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
         free( bucket );
         return 1;
   }
   return 0;
}

/* getters */
int get_zero_reach_size( void )
{
   return SizeOfList( memory_management->zero_reach_list );
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

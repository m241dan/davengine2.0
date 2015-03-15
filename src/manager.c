/* the memory manager written by Davenge using the D_HASH and DLIST *
 * D_HASH written by Davenge DLIST expanded on an existing library  */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "mud.h"

MEM_MANAGER *memory_management = NULL;

/* creators */

int init_manager( void )
{
   memory_management 			= malloc( sizeof( MEM_MANAGER ) );
   memory_management->zero_reach_list	= AllocList();
   memory_management->reach_list 	= init_hash( NUMERIC_HASH, MANAGER_SIZE );
   return 1;
}

MEM_BUCKET *new_bucket( MB_TYPE type, void *memory, size_t size )
{
   MEM_BUCKET *bucket;

   if( !memory_management )
   {
      bug( "%s: memory manager not initiliazed.\n", __FUNCTION__ );
      return NULL;
   }
   if( !memory )
   {
      bug( "%s: passed NULL memory to save. Returning NULL.\n", __FUNCTION__ );
      return NULL;
   }
   bucket 		= malloc( sizeof( MEM_BUCKET ) );
   bucket->memory	= memory;
   bucket->mem_size	= size;
   bucket->type		= type;
   bucket->reach	= AllocList();
   AttachToList( bucket, memory_management->zero_reach_list );
   return bucket;
}

/* ints */
int *new_integer( int num )
{
   size_t size = sizeof( int );
   int *ptr = malloc( size );
   *ptr = num;
   new_bucket( MEM_INTEGER, ptr, size );
   return ptr;
}


/* strings */
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
   size = length + STRING_PADDING;

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

/* buffers */
D_BUFFER *new_buffer( int width )
{
   D_BUFFER *buf;
   size_t size;

   size		= sizeof( D_BUFFER );
   buf		= malloc( size );
   buf->width 	= width;
   buf->favor 	= BOT_FAVOR;
   buf->lines	= NULL;
   assign( buf->lines, new_list() );
   setup_manager( buf, new_bucket( MEM_BUFFER, buf, size ) );
   return buf;
}

/* linked lists */
LLIST *new_list( void )
{
   LLIST *list;
   list 		= AllocList();
   setup_manager( list, new_bucket( MEM_LIST, list, sizeof( LLIST ) ) );
   return list;
}

D_HASH *new_hash( int type, int size )
{
   D_HASH *hash;
   hash		= init_hash( type, size );
   setup_manager( hash, new_bucket( MEM_HASH, hash, sizeof( D_HASH ) ) );
   return hash;
}

LUA_CHUNK *new_chunk( int size )
{
   LUA_CHUNK *chunk;

   if( size < 1 )
   {
      bug( "%s: attempting to create with size less than 1.", __FUNCTION__ );
      return NULL;
   }
   chunk 		= malloc( sizeof( LUA_CHUNK ) );
   chunk->chunk		= NULL;
   chunk->owner		= NULL;
   chunk->type		= VOT_UNOWNED;
   chunk->var_count	= 0;
   assign( chunk->chunk, new_hash( ASCII_HASH, size ) );
   setup_manager( chunk, new_bucket( MEM_CHUNK, chunk, sizeof( LUA_CHUNK ) ) );
   return chunk;
}

LUA_VAR *new_var( void )
{
   LUA_VAR *var;

   var 		= malloc( sizeof( LUA_VAR ) );
   var->owners	= NULL;
   var->name	= NULL;
   var->data	= 0;
   var->type	= TYPE_UNSET;
   var->script	= NULL;
   assign( var->owners, new_list() );
   setup_manager( var, new_bucket( MEM_VAR, var, sizeof( LUA_VAR ) ) );
   return var;
}

/* destroyers */
int free_bucket( MEM_BUCKET *bucket )
{
   if( !bucket )
   {
      bug( "%s: attempt to free NULL bucket.\n", __FUNCTION__ );
      return 0;
   }

   DetachFromList( bucket, memory_management->zero_reach_list );
   if( SizeOfList( bucket->reach ) > 0 )
   {
      bug( "%s: will not free bucket, still in reach somewhere.\n", __FUNCTION__ );
      /* put it into the hash */
      hash_add( memory_management->reach_list, bucket, (long)bucket->memory );
      return 0;
   }
   FreeList( bucket->reach );
   bucket->reach = NULL;

   switch( bucket->type )
   {
      default:
         bug( "%s: Invalid Memory Type for Bucket.", __FUNCTION__ );
         break;
      case MEM_INTEGER:
      case MEM_STRING:
         free( (char *)bucket->memory );
         break;
      case MEM_BUFFER:
         free_buffer( (D_BUFFER *)bucket->memory );
         break;
      case MEM_LIST:
      {
         LLIST *list = (LLIST *)bucket->memory;
         if( list->_iterators > 0 )
            bug( "%s: cannot free, list has unattached iterators.", __FUNCTION__ );
         else
            FreeList( (LLIST *)bucket->memory );
         break;
      }
      case MEM_HASH:
         free_hash( (D_HASH *)bucket->memory );
         break;
      case MEM_CHUNK:
         free_chunk( (LUA_CHUNK *)bucket->memory );
         break;
      case MEM_VAR:
         free_var( (LUA_VAR *)bucket->memory );
         break;
   }
   bucket->memory = NULL;
   free( bucket );
   return 1;
}

int free_buffer( D_BUFFER *buf )
{
   unassign( buf->lines );
   buf->_bucket = NULL;
   free( buf );
   return 1;
}

int free_chunk( LUA_CHUNK *chunk )
{
   chunk->owner = NULL;
   unassign( chunk->chunk );
   chunk->_bucket = NULL;
   free( chunk );
   return 1;
}

int free_var( LUA_VAR *var )
{
   unassign( var->owners );
   unassign( var->name );
   unassign( var->script );
   unreach_ptr( (void *)var->data, (void **)&var->data );
   var->_bucket = NULL;
   free( var );
   return 1;
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
      switch( bucket->type )
      {
         default: break;
         case MEM_BUFFER:
            ((D_BUFFER *)bucket->memory)->_reached = 1;
            break;
         case MEM_LIST:
         {
            LLIST *list = (LLIST *)bucket->memory;
            if( SizeOfList( list ) > 0 )
               reach_list_content( list );
            list->_reached = 1;
            break;
         }
         case MEM_HASH:
         {
            D_HASH *hash = (D_HASH *)bucket->memory;
            if( hash->count > 0 )
               reach_hash_content( hash );
            hash->_reached = 1;
            break;
         }
         case MEM_CHUNK:
            ((LUA_CHUNK *)bucket->memory)->_reached = 1;
            break;
         case MEM_VAR:
            ((LUA_VAR *)bucket->memory)->_reached = 1;
            break;
      }
   }
   AttachToList( assignment, bucket->reach );
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
      switch( bucket->type )
      {
         default: break;
         case MEM_BUFFER:
            ((D_BUFFER *)bucket->memory)->_reached = 0;
            break;
         case MEM_LIST:
         {
            LLIST *list = (LLIST *)bucket->memory;
            if( SizeOfList( list ) > 0 )
               unreach_list_content( list );
            list->_reached = 0;
            break;
         }
         case MEM_HASH:
         {
            D_HASH *hash = (D_HASH *)bucket->memory;
            if( hash->count > 0 )
               unreach_hash_content( hash );
            hash->_reached = 0;
         }
         case MEM_CHUNK:
            ((LUA_CHUNK *)bucket->memory)->_reached = 0;
            break;
         case MEM_VAR:
            ((LUA_VAR *)bucket->memory)->_reached = 0;
            break;
      }
   }
   DetachFromList( assignment, bucket->reach );
   return;
}

void reach_list_content( LLIST *list )
{
   CELL *cell_ptr;
   ITERATOR Iter;

   if( SizeOfList( list ) > 0 )
   {
      AttachIterator( &Iter, list );
      while( ( cell_ptr = NextCellInList( &Iter ) ) != NULL )
         reach_ptr( cell_ptr->_pContent, (void **)&cell_ptr->_pContent );
      DetachIterator( &Iter );
   }
}

void unreach_list_content( LLIST *list )
{
   CELL *cell_ptr;
   ITERATOR Iter;

   if( SizeOfList( list ) > 0 )
   {
      AttachIterator( &Iter, list );
      while( ( cell_ptr = NextCellInList( &Iter ) ) != NULL )
         unreach_ptr( cell_ptr->_pContent, (void **)&cell_ptr->_pContent );
      DetachIterator( &Iter );
   }
}

void reach_hash_content( D_HASH *hash )
{
   HASH_BUCKET *bucket;
   int x;

   for( x = 0; x < hash->size; x++ )
   {
      for( bucket = hash->array[x]; bucket; bucket = bucket->next )
         reach_ptr( bucket->data, (void **)&bucket->data );
   }
}

void unreach_hash_content( D_HASH *hash )
{
   HASH_BUCKET *bucket;
   int x;
   for( x = 0; x < hash->size; x++ )
   {
      for( bucket = hash->array[x]; bucket; bucket = bucket->next )
         unreach_ptr( bucket->data, (void **)&bucket->data );
   }
}

int reassign_native( MEM_BUCKET *bucket, void *ptr, size_t size )
{
   void **reacher;
   ITERATOR Iter;

   free( bucket->memory );
   bucket->memory = malloc( size );
   memcpy( bucket->memory, ptr, size );

   AttachIterator( &Iter, bucket->reach );
   while( ( reacher = (void **)NextInList( &Iter ) ) != NULL )
      *reacher = ptr;
   DetachIterator( &Iter );
   return 1;
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

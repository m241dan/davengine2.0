#include <stdlib.h>
#include "hash.h"

static inline void attach_bucket( void *to_add, long key, D_HASH *hash, long hash_key );

D_HASH *init_hash( int type, int size )
{
   D_HASH *hash;
   if( size < 2 )
   {
      printf( "%s: hash size must be at least 2.\r\n", __FUNCTION__ );
      return NULL;
   }
   switch( type )
   {
      default:
         printf( "%s: bad hash type, returning NULL.\r\n", __FUNCTION__ );
         return NULL;
      case NUMERIC_HASH:
      case ASCII_HASH:
         hash = (D_HASH *)malloc( sizeof( D_HASH ) );
         hash->type = type;
         hash->size = size;
         hash->array = (HASH_BUCKET **)calloc( size, sizeof( HASH_BUCKET * ) );
         break;
   }
   return hash;
}


void hash_add( D_HASH *hash, void *to_add, long key )
{
   long hash_key;

   if( !hash )
   {
      printf( "%s: cannot add to a NULL hash.\r\n", __FUNCTION__ );
      return;
   }
   if( !to_add )
   {
      printf( "%s: cannot add NULL data to hash.\r\n", __FUNCTION__ );
      return;
   }
   switch( hash->type )
   {
      default: printf( "%s: doesn'tknow how to add to this type of hash.\r\n", __FUNCTION__ ); break;
      case NUMBERIC_HASH:
         hash_key = key % hash->size;
         break;
      case ASCII_HASH:
         hash_key = ((char *)key)[0] % hash->size;
         break;
   }
   attach_bucket( to_add, key, hash, hash_key );
   return;
}

void hash_remove( D_HASH *hash, void *to_remove, long key )
{

}

void *hash_find( D_HASH *hash, long key )
{

}

void hash_show( D_HASH *hash )
{
   HASH_BUCKET *bucket;

   printf( "Hash Type: %d Size: %l\r\n", hash->type, hash->size );
   for( int x = 0; x < size; x++ )
   {
      printf( " - Bucket %d - \r\n", x );
      for( bucket = hash->array[x]; bucket; bucket = bucket->next )
      {
         switch( hash->type )
         {
            default: printf( "Invalid Hash Type.\r\n" ); break;
            case NUMERIC_HASH:
               printf( "%s Data at Key %l\r\n", !bucket->data ? "NULL" : "GOOD", bucket->key );
               break;
            case ASCII_HASH:
               printf( "%s Data at Key %s\r\n", !bucket->data ? "NULL" : "GOOD", (char *)bucket->key );
               break;
         }
   }
}

static inline void attach_bucket( void *to_add, long key, D_HASH *hash, long hash_key )
{
   HASH_BUCKET *bucket = init_bucket();
   bucket->data = to_add;
   bucket->key = key;
   if( hash[hash_key] )
      bucket->next = hash[hash_key];
   hash[hash_key] = bucket;
}


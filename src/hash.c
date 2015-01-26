#include <stdlib.h>
#include "hash.h"

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

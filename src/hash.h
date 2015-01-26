/* various hash data structures written by Daniel R. Koris */

typedef enum
{
   NUMERIC_HASH, ASCII_HASH, MAX_HASH_TYPE
} HASH_TYPES;

typedef struct d_hash
{
   unsigned char type;
   long 	 size;
   HASH_BUCKET **array;
} D_HASH;

typedef struct hash_bucket
{
   HASH_BUCKET  *next;
   void		*data;
   long		 key;
} HASH_BUCKET;

D_HASH 		*init_hash	( int type, int size );
HASH_BUCKET	*init_bucket	( void );
void  		 hash_add	( D_HASH *hash, void *to_add, long key );
void  		 hash_remove	( D_HASH *hash, void *to_remove, long key );
void 		*hash_find	( D_HASH *hash, long key );
void  		 hash_show	( D_HASH *hash );

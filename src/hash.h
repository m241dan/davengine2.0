/* various hash data structures written by Daniel R. Koris */

typedef struct hash_bucket HASH_BUCKET;
typedef struct d_hash	   D_HASH;

typedef enum
{
   NUMERIC_HASH, ASCII_HASH, MAX_HASH_TYPE
} HASH_TYPES;

struct hash_bucket
{
   HASH_BUCKET  *next;
   void		*data;
   long int	 key;
};

struct d_hash
{
   unsigned char type;
   long int	 size;
   HASH_BUCKET **hash_search;
   HASH_BUCKET **array;
};

#define init_bucket() (HASH_BUCKET *)calloc( 1, sizeof( HASH_BUCKET ) )
#define hash_find( hash, key ) __hash_find( hash, key, 1 )
#define hash_find_next( hash, key ) __hash_find( hash, key, 0 )

D_HASH 		*init_hash	( int type, int size );
int  		 hash_add	( D_HASH *hash, void *to_add, long key );
int  		 hash_remove	( D_HASH *hash, void *to_remove, long key );
HASH_BUCKET	*__hash_find	( D_HASH *hash, long key, int reset );
int		 hash_contains	( D_HASH *hash, void *to_find, long key );
void  		 hash_show	( D_HASH *hash );

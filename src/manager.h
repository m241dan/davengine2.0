/* This is the header file for the memory management library written by Davenge */

typedef struct memory_bucket MEM_BUCKET;
typedef struct memory_manager MEM_MANAGER;

#define MANAGER_SIZE 1001

typedef enum
{
   MEM_INTEGER, MEM_STRING, MAX_MEM
} MB_TYPE;

struct memory_bucket
{
   void 	*memory;
   unsigned char type;
   unsigned int	 reach;
};

struct memory_manager
{
   LLIST 	*zero_reach_list;
   D_HASH 	*reach_list;
};

#define assign( ptr, data )	\
{				\
   unreach_ptr( ptr );		\
   ptr = data;			\
   reach_ptr( ptr );		\
}

#define unassign( ptr )		\
{				\
   unreach_ptr( ptr );		\
   ptr = NULL;			\
}

/* creators */
int		 init_manager( void );
int		 new_bucket( MB_TYPE type, void *memory );
int 		*new_integer( int num );
char		*new_string( const char *fmt, ... );

/* destroyers */
int 		 free_bucket( MEM_BUCKET *bucket );

/* getters */
int 		 get_zero_reach_size( void );
MEM_BUCKET	*get_bucket_for( void *ptr );

/* utility */
void reach_ptr( void *ptr );
void unreach_ptr( void *ptr );

/* monitor */
void 		 clear_zero_reach( void );

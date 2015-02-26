/* This is the header file for the memory management library written by Davenge */

typedef struct memory_bucket MEM_BUCKET;
typedef struct memory_manager MEM_MANAGER;

#define MANAGER_SIZE 1001
#define STRING_PADDING 101

typedef enum
{
   MEM_INTEGER, MEM_STRING, MEM_BUFFER, MAX_MEM
} MB_TYPE;

struct memory_bucket
{
   void 	*memory;
   size_t	 mem_size;
   unsigned char type;
   LLIST	*reach;
};

struct memory_manager
{
   LLIST 	*zero_reach_list;
   D_HASH 	*reach_list;
};

#define assign( ptr, data )		\
{					\
   unreach_ptr( ptr,(void **)&ptr );	\
   ptr = data;				\
   reach_ptr( ptr, (void **)&ptr );	\
}

#define unassign( ptr )			\
{					\
   unreach_ptr( ptr, (void **)&ptr );	\
   ptr = NULL;				\
}

/* creators */
int		 init_manager		( void );
int		 new_bucket		( MB_TYPE type, void *memory, size_t size );
int 		*new_integer		( int num );
char		*new_string		( const char *fmt, ... );
char		*str_alloc		( size_t size );
D_BUFFER	*new_buffer		( int width );

/* destroyers */
int 		 free_bucket		( MEM_BUCKET *bucket );
int		 free_buffer		( D_BUFFER *buf );

/* getters */
int 		 get_zero_reach_size	( void );
MEM_BUCKET	*get_bucket_for		( const void *ptr );
size_t		 get_size		( const void *ptr );

/* utility */
void 		 reach_ptr		( const void *ptr, void **assignment );
void 		 unreach_ptr		( const void *ptr, void **assignment );

/* monitor */
void 		 clear_zero_reach	( void );

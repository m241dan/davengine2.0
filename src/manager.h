/* This is the header file for the memory management library written by Davenge */

typedef struct memory_bucket 	MEM_BUCKET;
typedef struct memory_manager 	MEM_MANAGER;

#define MANAGER_SIZE 1001
#define STRING_PADDING 101

typedef enum
{
   MEM_INTEGER, MEM_STRING, MEM_BUFFER, MEM_LIST, MEM_HASH, MEM_CHUNK, MEM_VAR, MAX_MEM
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
   LLIST	*zero_reach_list;
   D_HASH 	*reach_list;
};

#define setup_manager( ptr, bucket ) 	\
do {					\
   (ptr)->_bucket = (bucket);		\
   (ptr)->_reached = 0;			\
} while(0)

#define assign( ptr, data )			\
do {						\
   unreach_ptr( (ptr),(void **)&(ptr) );	\
   (ptr) = (data);				\
   reach_ptr( (ptr), (void **)&(ptr) );		\
} while(0)

#define unassign( ptr )				\
do {						\
   unreach_ptr( (ptr), (void **)&(ptr) );	\
   ptr = NULL;					\
} while(0)

#define is_reachable( data ) (data)->_bucket ? TRUE : FALSE
#define is_reached( data )  (data)->_reached ? TRUE : FALSE

#define LINK(link, first, last, next, prev) \
do                                          \
{                                           \
   if ( !(first) )                          \
   {                                        \
      (first) = (link);                     \
      (last) = (link);                      \
   }                                        \
   else                                     \
      (last)->next = (link);                \
   (link)->next = NULL;                     \
   if ((first) == (link))                   \
      (link)->prev = NULL;                  \
   else                                     \
      (link)->prev = (last);                \
   (last) = (link);                         \
} while(0)

#define INSERT(link, insert, first, next, prev) \
do                                              \
{                                               \
   (link)->prev = (insert)->prev;               \
   if ( !(insert)->prev )                       \
      (first) = (link);                         \
   else                                         \
      (insert)->prev->next = (link);            \
   (insert)->prev = (link);                     \
   (link)->next = (insert);                     \
} while(0)

#define UNLINK(link, first, last, next, prev)   \
do                                              \
{                                               \
   if ( !(link)->prev )                         \
   {                                            \
      (first) = (link)->next;                   \
      if ((first))                              \
         (first)->prev = NULL;                  \
   }                                            \
   else                                         \
   {                                            \
      (link)->prev->next = (link)->next;        \
   }                                            \
   if ( !(link)->next )                         \
   {                                            \
      (last) = (link)->prev;                    \
      if((last))                                \
         (last)->next = NULL;                   \
   }                                            \
   else                                         \
   {                                            \
      (link)->next->prev = (link)->prev;        \
   }                                            \
} while(0)

/* creators */
int		 init_manager		( void );
MEM_BUCKET	*new_bucket		( MB_TYPE type, void *memory, size_t size );
int 		*new_integer		( int num );
char		*new_string		( const char *fmt, ... );
char		*str_alloc		( size_t size );
D_BUFFER	*new_buffer		( int width );
LLIST		*new_list		( void );
D_HASH		*new_hash		( int type, int size );
LUA_CHUNK	*new_chunk		( int size );
LUA_VAR		*new_var		( void );

/* destroyers */
int 		 free_bucket		( MEM_BUCKET *bucket );
int		 free_buffer		( D_BUFFER *buf );
int		 free_chunk		( LUA_CHUNK *chunk );
int		 free_var		( LUA_VAR *var );

/* getters */
int 		 get_zero_reach_size	( void );
MEM_BUCKET	*get_bucket_for		( const void *ptr );
size_t		 get_size		( const void *ptr );

/* utility */
void 		 reach_ptr		( const void *ptr, void **assignment );
void 		 unreach_ptr		( const void *ptr, void **assignment );
void		 reach_list_content	( LLIST *list );
void		 unreach_list_content	( LLIST *list );
void		 reach_hash_content	( D_HASH *hash );
void		 unreach_hash_content	( D_HASH *hash );
int		 reassign_native	( MEM_BUCKET *bucket, void *ptr, size_t size ); /* for use on native C datatypes only */
/* monitor */
void 		 clear_zero_reach	( void );



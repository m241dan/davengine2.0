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
   int		 reach;
};

struct memory_manager
{
   LIST 	*zero_reach_list;
   D_HASH 	*reach_list;
};

/* creators */
int		 init_manager( void );
int		 new_bucket( MB_TYPE type, void *memory );
int 		*new_integer( int num );
char		*new_string( const char *fmt, ... );

/* destroyers */
int 		 free_bucket( MEM_BUCKET *bucket );

/* getters */
int 		 get_zero_reach_size( void );


/* monitor */
void 		 clear_zero_reach( void );

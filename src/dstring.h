/* header file for dstring.c aka Davenge String */
/* written by Davenge */

typedef struct davenge_string		DSTRING;
typedef struct raw_davengine_string	RAW_DSTRING;
typedef struct davenge_string **	STRING_ARRAY;
typedef struct garbage_can		GARBAGE_CAN;

typedef long long int lint;
#define DONT_COLLECT	-69
#define DEFAULT_LIFE    10
#define rawstr( string_ptr ) 	( (string_ptr)->raw_data->data )
#define str( string_ptr )	( (string_ptr)->raw_data->data + (string_ptr)->cursor )

/* global can */
extern GARBAGE_CAN *string_garbage_can;

struct garbage_can
{
   DSTRING *collection_bucket;
   int count;
};

struct raw_davengine_string
{
   char *data;
   size_t size;
   lint length;
   int reach;
};

struct davenge_string
{
   /* single linked list, for economics */
   DSTRING *next_gc;
   RAW_DSTRING *raw_data;
   int life;
   unsigned int cursor; /* used for parsing */
   /* for arrays */
   STRING_ARRAY array;
   int position;
   int a_size;
};

/* constructors */
RAW_DSTRING 	*new_raw_string		( const char *string );
DSTRING 	*new_string		( const char *fmt, ... );
DSTRING 	*new_string_nogc	( const char *fmt, ... );
GARBAGE_CAN 	*setup_garbage_can	( void );

/* deconstructors */
void free_dstring( DSTRING *string );
void free_raw_string( RAW_DSTRING *raw_string );
void takeout_can( void );

/* actions */
void toss_into_can( DSTRING *string );
void takeout_string( DSTRING *trash );
void attach_string( DSTRING *string );
void detach_string( DSTRING *string );

/* checkers */
int check_bucket_for_dstring( DSTRING *string );

/* the trash man! */
void trash_man( void );
void takeout_can( void ); /* clear out the damn trashcan */

/* utility */
void update_collection( DSTRING *string, int life );
void inspect_can( void );

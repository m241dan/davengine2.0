/* header file for dstring.c aka Davenge String */
/* written by Davenge */

typedef struct davengine_string 	DSTRING;
typedef struct raw_davengine_string	RAW_DSTRING;
typedef struct davenge_string **	STRING_ARRAY;
typedef struct garbage_can		GARBAGE_CAN;

typedef long long int lint;
#define DONT_COLLECT	-69

extern GARBAGE_CAN *string_garbage_can;

struct garbage_can
{
   DSTRING *collection_bucket;
   DSTRING *no_collection_bucket
   int collection_count;
   int no_collection_count;
}

struct raw_davengine_string
{
   char *data;
   size_t size;
   lint length;
   int reach;
}

struct davenge_string
{
   /* single linked list, for economics */
   DSTRING *next_gc;
   RAW_DSTRING *raw_data;
   int life;
   /* for arrays */
   STRING_ARRAY array;
   int position;
   int a_size;
}

/* constructors */
RAW_DSTRING 	*new_raw_string		( const char *string );
DSTRING 	*new_string		( const char *fmt, ... );
DSTRING 	*new_string_nogc	( const char *fmt, ... );
GARBAGE_CAN 	*setup_garbage_can	( void );

/* deconstructors */
void free_dstring( DSTRING *string );
void free_raw_string( RAW_DSTRING *raw_string );
void takeout_can( GARBAGE_CAN *can );

/* setters */
void set_for_collection( DSTRING *string, int life );

/* actions */
void toss_into_can( DSTRING *string );
void update_collection( DSTRING *string, int life );
void takeout_string( DSTRING *trash );

/* checkers */
int raw_check_bucket_for_dstring( DSTRING *bucket, DSTRING *string );


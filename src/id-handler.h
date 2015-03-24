/* id-handlers.h written by Davenge */

struct id_handler
{
   HAMDLER_TYPE	 type;
   char 	*name;
   int		 top_id;
   bool		 can_recycle;
   LLIST	*recycled_ids;
};

struct id_tag
{
   HANDLER_TYPE	 type;
   int 		 id;
   char 	*created_by;
   char 	*created_on;
   char 	*modified_by;
   char 	*modified_on;
};

typedef enum
{
   NONE_IDS, ACCOUNT_IDS, MAX_ID_HANDLER
} HANDLER_TYPE;

extern ID_HANDLER *handlers[MAX_ID_HANDLER];

/* handlers */

/* creation / deletion */
ID_HANDLER	*init_handler		( void );
int		 free_handler		( ID_HANDLER *handler );

/* database */
int		 load_id_handlers	( void );
int		 load_recycled_ids	( void );

/* id tag */

/* creation / deletion */
ID_TAG		*init_tag		( void );
int		 free_tag		( ID_TAG *tag );
int		 delete_tag		( ID_TAG *tag );
int		 new_tag		( ID_TAG *tag, const char *creator );

/* database */
int		 db_load_tag		( ID_TAG *tag, MYSQL_ROW *row );
int		 update_tag		( ID_TAG *tag, const char *effector, ... );

/* getters */
int		 get_top_id		( int type );
int		 get_new_id		( int type );
int		 get_potential_id	( int type );

/* utility */
ID_TAG		*copy_tag		( ID_TAG *tag );

#include "mud.h"


/* handlers */

/* creation / deletion */
ID_HANDLER *init_handler( void )
{
   ID_HANDLER *handler;
   handler 			= (ID_HANDLER *)malloc( sizeof( ID_HANDLER ) );
   handler->recycled_ids 	= AllocList();
   handler->type		= NONE_IDS;
   handler->top_id		= 0;
   handler->can_recycle		= FALSE;
   handler->name 		= NULL;
   return handler;
}

int free_handle( ID_HANDLER *handler )
{
   ITERATOR Iter;

   unassign( handler->name );
   unassign( handler->can_recycle );
   return 1;
}

/* database */
int              load_id_handlers       ( void );
int              load_recycled_ids      ( void );

/* id tag */

/* creation / deletion */
ID_TAG          *init_tag               ( void );
int              free_tag               ( ID_TAG *tag );
int              delete_tag             ( ID_TAG *tag );
int              new_tag                ( ID_TAG *tag, const char *creator );

/* database */
int              db_load_tag            ( ID_TAG *tag, MYSQL_ROW *row );
int              update_tag             ( ID_TAG *tag, const char *effector, ... );

/* getters */
int              get_top_id             ( int type );
int              get_new_id             ( int type );
int              get_potential_id       ( int type );

/* utility */
ID_TAG          *copy_tag               ( ID_TAG *tag );


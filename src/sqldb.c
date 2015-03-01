/* The library for the engine that will deal with sql interactions written by Davenge */

#include <stdlib.h>
#include "mud.h"

void connect_to_sql( void )
{
   if( ( sql_handle = mysql_init(NULL) ) == NULL )
   {
      bug( "Could not initialize mysql connection: %s", mysql_error( sql_handle ) );
      exit(1);
   }

   if( mysql_real_connect( sql_handle, DB_ADDR, DB_LOGIN, DB_PASSWORD, DB_NAME, 0, NULL, 0 ) == NULL )
   {
      bug( "Could not connect to database: %s", mysql_error( sql_handle ) );
      mysql_close( sql_handle );
      exit(1);
   }
}

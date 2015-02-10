/* Davengine Lua C API utilities written by Davenge */

#include <stdlib.h>
#include "mud.h"

/* creator */

void init_lua_handle( void )
{
   if( ( lua_handle = luaL_newstate() ) == NULL )
   {
      bug( "Could not initialize the Lua handle." );
      exit(1);
   }
   luaL_openlibs( lua_handle );
}

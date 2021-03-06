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
   log_string( "Loading Lua Libs" );
   luaL_openlibs( lua_handle );
}

void load_lua_server_scripts( void )
{
   int ret, top = lua_gettop( lua_handle );

   if( luaL_loadfile( lua_handle, SERVER_SCRIPT ) || ( ret = lua_pcall( lua_handle, 0, 0, 0 ) ) )
   {
      bug( "%s: could not load server settings script.\r\n - Ret: %d - %s", __FUNCTION__, ret, lua_tostring( lua_handle, -1 ) );
      return;
   }
   /* server configs */
   lua_getglobal( lua_handle, "mudport" );
   MUD_PORT = lua_tonumber( lua_handle, -1 ) ;
   lua_getglobal( lua_handle, "tick" );
   PULSES_PER_SECOND = lua_tonumber( lua_handle, -1 );
   lua_getglobal( lua_handle, "mud_name" );
   assign( MUD_NAME, new_string( "%s", lua_tostring( lua_handle, -1 ) ) );

   /* database configs */
   lua_getglobal( lua_handle, "db_name" );
   assign( DB_NAME, new_string( "%s", lua_tostring( lua_handle, -1 ) ) );
   lua_getglobal( lua_handle, "db_addr" );
   assign( DB_ADDR, new_string( "%s", lua_tostring( lua_handle, -1 ) ) );
   lua_getglobal( lua_handle, "db_login" );
   assign( DB_LOGIN, new_string( "%s", lua_tostring( lua_handle, -1 ) ) );
   lua_getglobal( lua_handle, "db_pass" );
   assign( DB_PASSWORD, new_string( "%s", lua_tostring( lua_handle, -1 ) ) );

   lua_settop( lua_handle, top );
}

/* lua_var library to handle lua_var chunks written by Davenge */
#include <string.h>
#include "mud.h"

/* setters */
bool set_var_to_chunk( LUA_VAR *var, LUA_CHUNK *chunk )
{
   if( get_var_from_chunk( chunk, var->name ) )
   {
      bug( "%s: variable with the name %s already in the chunk, cannot assign.", __FUNCTION__, var->name );
      return FALSE;
   }
   if( !hash_add( chunk->chunk, var, (long)var->name ) )
   {
      bug( "%s: could not add var to chunk.", __FUNCTION__ );
      return FALSE;
   }
   AttachToList( chunk, var->owners );
   chunk->var_count++;
   /* sql stuff */
   return TRUE;
}

bool rem_var_from_chunk( LUA_VAR *var, LUA_CHUNK *chunk )
{
   if( !hash_remove( chunk->chunk, var, (long)var->name ) )
   {
      bug( "%s: could not remove var from chunk.", __FUNCTION__ );
      return FALSE;
   }
   DetachFromList( chunk, var->owners );
   chunk->var_count--;
   /* sql stuff */
   return TRUE;
}


bool set_chunk_to_entity( LUA_CHUNK *chunk, void *entity )
{
   return FALSE;
}

bool set_var_name(LUA_VAR *var, char *name )
{
   LUA_CHUNK *chunk;
   ITERATOR Iter;

   if( !var )
   {
      bug( "%s: NULL var passed.", __FUNCTION__ );
      return FALSE;
   }
   if( !name || name[0] == '\0' )
   {
      bug( "%s: bad NAME passed.", __FUNCTION__ );
      return FALSE;
   }

   if( SizeOfList( var->owners ) > 0 )
   {
      AttachIterator( &Iter, var->owners );
      while( ( chunk = (LUA_CHUNK *)NextInList( &Iter ) ) != NULL )
         hash_remove( chunk->chunk, var, (long)var->name );
      DetachIterator( &Iter );
   }
   assign( var->name, new_string( name ) );
   if( SizeOfList( var->owners ) > 0 )
   {
      AttachIterator( &Iter, var->owners );
      while( ( chunk = (LUA_CHUNK *)NextInList( &Iter ) ) != NULL )
         hash_add( chunk->chunk, var, (long)var->name );
      DetachIterator( &Iter );
   }
   /* sql stuff */
   return TRUE;
}

bool set_var_script( LUA_VAR *var, char *path )
{
   assign( var->script, path );
   /* sql stuff */
   return TRUE;
}

bool set_var_to_string( LUA_VAR *var, char *string )
{
   if( get_bucket_for( string ) == NULL )
   {
      bug( "%s: cannot add unmanged string to var.", __FUNCTION__ );
      return FALSE;
   }
   if( var->type != TYPE_INT && var->type != TYPE_UNSET )
      unreach_ptr( (void *)var->data, (void **)&var->data );
   var->type = TYPE_STRING;
   var->data = (long)string;
   reach_ptr( (void *)var->data, (void **)&var->data );
   /* sql stuff */
   return TRUE;
}

bool set_var_to_int( LUA_VAR *var, long int value )
{
   if( var->type != TYPE_INT && var->type != TYPE_UNSET )
      unreach_ptr( (void *)var->data, (void **)&var->data );
   var->type = TYPE_INT;
   var->data = value;
   /* sql stuff */
   return TRUE;
}

/* getters */
LUA_VAR *get_var_from_chunk( LUA_CHUNK *chunk, char *name )
{
   HASH_BUCKET *bucket;
   LUA_VAR *var;

   if( ( bucket = hash_find( chunk->chunk, (long)name ) ) == NULL )
      return NULL;
   do
   {
      var = (LUA_VAR *)bucket->data;
      if( !strcmp( var->name, name ) )
      {
         printf( "found\r\n" );
         break;
      }
   } while( ( var = NULL ) && ( bucket = hash_find_next( chunk->chunk, (long)name ) ) != NULL );
   return var;
}

char *get_var_string( LUA_VAR *var )
{
   if( var->type != TYPE_STRING )
   {
      bug( "%s: var not of type string.", __FUNCTION__ );
      return NULL;
   }
   return ((char *)var->data );
}

int get_var_int( LUA_VAR *var )
{
   if( var->type != TYPE_INT )
   {
      bug( "%s: var not of type int.", __FUNCTION__ );
      return 0;
   }
   return ((int)var->data );
}

long get_var_long( LUA_VAR *var )
{
   if( var->type != TYPE_INT )
   {
      bug( "%s: var not of typeint.", __FUNCTION__ );
      return 0;
   }
   return var->data;
}

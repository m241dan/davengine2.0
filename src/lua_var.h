/* header file for lua_var chunk written by Davenge */

typedef enum
{
   TYPE_UNSET, TYPE_INT, TYPE_STRING, MAX_VAR_TYPE
} VAR_TYPE;

typedef enum
{
   VOT_UNOWNED, VOT_ENTITY, MAX_VOT
} VAR_OWNER_TYPE;

struct lua_var_chunk
{
   void			*owner;
   VAR_OWNER_TYPE	 type;
   D_HASH 		*chunk;
   int		 	 var_count;
   void		        *_bucket;
   int			 _reached;
};

struct lua_var
{
   LLIST	*owners; /* list of chunks it belongs to */
   char		*name;
   long		 data;
   VAR_TYPE	 type;
   char		*script;
   void		*_bucket;
   int		 _reached;
};

/* setters */
bool	 set_var_to_chunk	( LUA_VAR *var, LUA_CHUNK *chunk );
bool	 set_chunk_to_entity	( LUA_CHUNK *chunk, void *entity );
bool     set_var_name		( LUA_VAR *var, char *name );
bool     set_var_to_string	( LUA_VAR *var, char *string );
bool	 set_var_to_int		( LUA_VAR *var, long int value );

/* getters */
LUA_VAR	*get_var_from_chunk	( LUA_CHUNK *chunk, char *name );

/* The header file for the sqldb.c library written by Davenge */

extern MYSQL *sql_handle;
extern char  *DB_NAME;
extern char  *DB_ADDR;
extern char  *DB_LOGIN;
extern char  *DB_PASSWORD;

void connect_to_sql( void );

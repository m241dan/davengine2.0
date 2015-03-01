/* buffer library for mud outputs written by Davenge*/

#include <ctype.h>
#include <string.h>
#include "mud.h"

/* utility */
int parse_into_buffer( D_BUFFER *buf, char *input )
{
   int real, x;
   bool newline, space;

   while( *input != '\0' )
   {
      newline = FALSE;
      space = FALSE;
      /* get the real length */
      real = find_real_length( input, buf->width );
      /* search out any new lines */
      for( x = 0; x < real; x++ )
      {
         if( input[x] == '\n' || input[x] == '\r' )
         {
            newline = TRUE;
            real = x;
            break;
         }
      }
      /* from the end, find a blank space to terminate at if not alreadyat one */
      if( !newline )
      {
         for( ; x > 0; x-- )
         {
            if( isspace( input[x] ) )
            {
               space = TRUE;
               real = x;
               break;
            }
         }
      }
      /* create line from the new "real length" */
      AttachToEnd( copy_string_fl( input, real ), buf->lines );
      /* increment the input pointer */
      input += real;
      if( newline )
      {
         if( *input == '\n' || *input == '\r' )
            input++;
         if( *input == '\n' || *input == '\r' )
            input++;
      }
      if( space && isspace( *input ) )
         input++;
   }
   return 1;
}

char *buffer_to_string( D_BUFFER *buf )
{
   char *string, *line;
   ITERATOR Iter;
   int length;

   /* raw length + \n\r for each line + padding */
   length = get_buffer_length( buf ) + ( SizeOfList( buf->lines ) * 2 ), + STRING_PADDING;
   /* +1 for NULL terminator */
   string = str_alloc( length + 1 );

   AttachIterator( &Iter, buf->lines );
   while( ( line = (char *)NextInList( &Iter ) ) != NULL )
   {
      strcat( string, line );
      strcat( string, "\n\r" );
   }
   DetachIterator( &Iter );
   string[length] = '\0';
   return string;
}

int get_buffer_length( D_BUFFER *buf )
{
   char *line;
   ITERATOR Iter;
   int length = 0;

   AttachIterator( &Iter, buf->lines );
   while( ( line = (char *)NextInList( &Iter ) ) != NULL )
      length += strlen( line );
   DetachIterator( &Iter );
   return length;
}

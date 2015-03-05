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

int set_favor( D_BUFFER *buf, BUFFER_FAVOR favor )
{
   buf->favor = favor;
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
      strcat( string, "\r\n" );
   }
   DetachIterator( &Iter );
   string[length] = '\0';
   return string;
}

char *buffers_to_string( D_BUFFER *buffers[], int size )
{
   ITERATOR Iter[size];
   char *string, *line;
   int print_start[size];
   int width, max_lines, x, y, padding;

   /* find the raw length of all the buffers added together *
    * and find the max lines and attach iterators */
   for( x = 0, width = 0, max_lines = 0; x < size; x++ )
   {
      if( SizeOfList( buffers[x]->lines ) > max_lines )
         max_lines = SizeOfList( buffers[x]->lines );
      width += buffers[x]->width;
      AttachIterator( &Iter[x], buffers[x]->lines );
   }
   /* figure out the bounds for bot/mid/top favored buffers */
   for( x = 0; x < size; x++ )
   {
      if( SizeOfList( buffers[x]->lines ) == max_lines || buffers[x]->favor == TOP_FAVOR )
      {
         print_start[x] = 0;
         continue;
      }
      else
         print_start[x] = max_lines - SizeOfList( buffers[x]->lines );
      if( buffers[x]->favor == MID_FAVOR )
         print_start[x] /= 2;
   }
   string = str_alloc( ( ( width * max_lines ) * STRING_PADDING ) );
   /* iterate through and create the string */
   printf( "%s: max_lines %d\r\n", __FUNCTION__, max_lines );
   for( x = 0; x < size; x++ )
   {
      printf( "%s: buffer %d lines %d print_start %d\r\n", __FUNCTION__, x, SizeOfList( buffers[x]->lines ), print_start[x] );
   }
   for( y = 0; y < max_lines; y++ )
   {
      for( x = 0; x < size; x++ )
      {
         if( print_start[x] <= y && y < SizeOfList( buffers[x]->lines ) )
         {
            if( ( line = (char *)NextInList( &Iter[x] ) ) != NULL )
            {
               strcat( string, line );
               if( ( padding = buffers[x]->width - strlen( line ) ) != 0 )
                  strcat( string, create_pattern( " ", padding ) );
            }
            else
               strcat( string, create_pattern( " ", buffers[x]->width ) );
         }
         else
            strcat( string, create_pattern( " ", buffers[x]->width ) );
      }
      strcat( string, "\r\n" );
   }
   /* terminate, remove iterators and return */
   for( x = 0; x < size; x++ )
      DetachIterator( &Iter[x] );
   string[width * max_lines] = '\0';
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

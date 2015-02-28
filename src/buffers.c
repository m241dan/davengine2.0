/* buffer library for mud outputs */

#include "mud.h"

/* utility */
/*
int parse_into_buffer( D_BUFFER *buf, char *input )
{
   char *ptr, *line;
   size_t size = ( buf->width * 3 ) + 1; * 3 three because each character code be a color, #rR = deep red 'R', three characters AND +1 for NULL terminator *
   enum {
      MODE_TEST, MODE_COPY, MODE_FIND_SPACE
   } PARSE_MODE;
   PARSE_MODE mode;
   int x, parse_amount, noncolor_count;

   if( !input || input[0] == '\0' )
   {
      bug( "%s: passed a blank or NULL input.", __FUNCTION__ );
      return 0;
   }

   for( mode = MODE_TEST; *input == '\0'; )
   {
      switch( mode );
      {
         case MODE_TEST:
            for( x = 0, noncolor_count = 0; ; x++)
            {
               * create new lines on...*
               if( input[x] == '\n' )
               {
                  parse_amount = x;
                  mode = MODE_COPY;
                  break;
               }
               * create a new line, but don't copy the NULL terminator *
               if( input[x] == '\0' )
               {
                  parse_amount = x - 1;
                  mode = MODE_COPY;
                  break;
               }

               if( input[x] == '#' )
               {
                  * if its a valid color code, skip it *
                  if( input[x+1] != '\0' &&  input[x+1] != '#' )
                  {
                     x += 2;
                     continue;
                  }
                  * its a double hashtag, so it counts as 1 real character but we need to take both so it doesn't get parsed out at color translation time *
                  if( input[x+1] != '\0' )
                  {
                     x += 2;
                     noncolor_count++;
                  }
               }
               else
                  noncolor_count++;


               * check out amount of actual characters *
               if( noncolor_count == buf->width )
               {
                  parse_amount = x;
                  mode= MODE_FIND_SPACE;
                  break;
               }
            }
      }
   }
} */

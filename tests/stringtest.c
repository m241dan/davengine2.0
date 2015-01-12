#include <stdlib.h>
#include <stdio.h>
#include "../src/dstring.h"

void main()
{
   string_garbage_can = setup_garbage_can();

   DSTRING *string1 = new_string( "A string" );
   DSTRING *string2 = new_string( "A string" );
   DSTRING *string3 = new_string( "A disimilar string" );
   DSTRING *string4 = new_string( "same len" );
   DSTRING *string5 = new_string( "A str" );
   DSTRING *string6 = new_string( "AN UPPER STRING" );
   DSTRING *string7 = new_string( "a lower string" );
   DSTRING *string8 = new_string( "a capItalizeD sTring" );

   if( compare( string1, string2 ) )
      printf( "Test 1 success.\r\n" );
   if( compare( string1, string1 ) )
      printf( "Test 2 success.\r\n" );
   if( !compare( string1, string3 ) )
      printf( "Test 3 success.\r\n" );
   if( !compare( string2, string4 ) )
      printf( "Test 4 success.\r\n" );

   string2->cursor++;
   if( !compare( string1, string2 ) )
      printf( "Test 5 success.\r\n" );
   string1->cursor += 2;
   if( !compare( string1, string2 ) )
      printf( "Test 6 success.\r\n" );
   if( raw_compare( string1, string2 ) )
      printf( "Test 7 success.\r\n" );
   if( compare1( string1, "string" ) )
      printf( "Test 8 success.\r\n" );
   if( compare1( string2, " string" ) )
      printf( "Test 9 success.\r\n" );
   if( !compare1( string3, "a disimilar string" ) )
      printf( "Test 10 success.\r\n" );
   if( raw_compare1( string1, "A string" ) )
      printf( "Test 11 success.\r\n" );
   if( !compare_prefix( string1, string3 ) )
      printf( "Test 12 success.\r\n" );
   if( raw_compare_prefix( string1, string5 ) )
      printf( "Test 13 success.\r\n" );
   if( !compare_prefix1( string1, "A " ) )
      printf( "Test 14 success.\r\n" );
   if( compare_prefix1( string1, "str" ) )
      printf( "Test 15 success.\r\n" );
   if( raw_compare_prefix1( string1, "A str" ) )
      printf( "Test 16 succes.\r\n" );

   DSTRING *soft_string = softcopy( string1 );
   if( soft_string->raw_data == string1->raw_data )
      printf( "Test 17 success.\r\n" );

   DSTRING *soft_string_nogc = softcopy_nogc( string1 );
   printf( "There should not be more than 4 \"A string\", in the can, if there are, the problem could be with softcopy_nogc\r\n" );
   DSTRING *soft_string_lit = softcopy_lit( string1 );

   printf( "'%s' if this is all lowercase, test 18 is a success.\r\n", str( downcase( string6 ) ) );
   downcase1( string6 );
   printf( "'%s' if this is all lowercase, test 19 is a success.\r\n", str( string6 ) );

   printf( "'%s' if this is all uppercase, test 20 is a success.\r\n", str( uppercase( string7 ) ) );
   uppercase1( string7 );
   printf( "'%s' if this is all uppercase, test 21 is a success.\r\n", str( string7 ) );

   printf( "'%s' if the first letter of each word only is capitalized, test 22 is a success.\r\n", str( capitalize( string8 ) ) );
   capitalize1( string8 );
   printf( "'%s' if the first letter of each word only is capitalized, test 23 is a success.\r\n", str( string8 ) );

   printf( "Max Test is 23.\r\n" );

   printf( "Can Inspection is Off.\r\n" );
/*
   while( 1 )
   {
      trash_man();
      inspect_can();
      if( string_garbage_can->count == 0 )
         break;
   }
*/
   printf( "Test Complete.\r\n" );
}

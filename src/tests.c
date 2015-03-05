/* This is a library of tests for the mud written by Davenge */

#include <string.h>
#include "mud.h"

void find_real_length_test( void )
{
   int real;
   /* test 1 */
   real = find_real_length( "#gT#rh#Ri#bs", 4 );
   if( real != 12 )
      bug( "%s: Test 1 Failed. Real %d should be 12.", __FUNCTION__, real );
   else
      bug( "%s: Test 1 Passed.", __FUNCTION__ );

   /* test 2 */
   real = find_real_length( "##Hashta#rg", 8 );
   if( real != 11 )
      bug( "%s: Test 2 Failed. Real %d should be 11", __FUNCTION__, real );
   else
      bug( "%s: Test 2 Passed.", __FUNCTION__ );

   real = find_real_length( "ThisEnding#", 12 );
   if( real != 10 )
      bug( "%s: Test 3 Failed. Real %d should be 10", __FUNCTION__, real );
   else
      bug( "%s: Test 3 Passed.", __FUNCTION__ );

   real = find_real_length( "#rT#zhis is a #CColor #pString", 12 );
   if( real != 18 )
      bug( "%s: Test 4 Failed. Real %d should be 18", __FUNCTION__, real );
   else
      bug( "%s: Test 4 Passed.", __FUNCTION__ );
}

void copy_string_fl_test( void )
{
   char *string = copy_string_fl( "#rT#zhis is a #CColor #pString", 18 );
    if( !strcmp( string, "#rT#zhis is a #CCo" ) )
      bug( "%s: Test 1 Passed.", __FUNCTION__ );
   else
      bug( "%s: Test 1 Failed. Result = %s\n", __FUNCTION__, string );
}

void buffer_basics_test( void )
{
   D_BUFFER *buf;

   create_buffer( buf, 20, "This is a test string that is meant\n\rto wrap in a few places.\r\n" );
   bug( "%s: The measuring stick -\r\nThis is a test string that is meant\n\rto wrap in a few places.\r\n.", __FUNCTION__ );
   bug( "%s: Function Output -\r\n%s.", __FUNCTION__, buffer_to_string( buf ) );
   bug( "%s: If the above two strings are the same, the test passed.", __FUNCTION__ );
}

void buffer_basics_test2( void )
{
   D_BUFFER *buf[NUM_BUFFERS];

   create_buffer( buf[0], 22, "H:127/127 M: 200/200 \r\n" );
   create_buffer( buf[1], 50, "This is just a simple little test to see how prompt and buffers will interact\r\n" );
   create_buffer( buf[2], 50, "/----------------------------\\\r\n|                            |\r\n\\----------------------------/\r\n" );
   set_favor( buf[0], BOT_FAVOR );
   set_favor( buf[1], TOP_FAVOR );
   bug( "%s: Does this look right?\r\n\r\n%s", __FUNCTION__, buffers_to_string( buf, 3 ) );
   
}

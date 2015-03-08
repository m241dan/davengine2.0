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
   create_buffer( buf[1], 50, "This is a simple test to see how prompt and buffers will interact. At first you'll see a 'dummy map' on the right. It's only supposed to be three lines with the mid line being blank. On the left, you'll see an example of a bottom favored buffer only printing out its small output at the bottom. The map on the right is mid favored.\r\n" );
   create_buffer( buf[2], 50, " /----------------------------\\\r\n |                            |\r\n \\----------------------------/\r\n" );
   set_favor( buf[0], BOT_FAVOR );
   set_favor( buf[1], TOP_FAVOR );
   set_favor( buf[2], MID_FAVOR );
   bug( "%s: Does this look right?\r\n\r\n%s\r\n.", __FUNCTION__, buffers_to_string( buf, 3 ) );
}

void lua_chunk_test( void )
{
   LUA_CHUNK *chunk;
   LUA_VAR *var;

   chunk = new_chunk( 52 );

   var = new_var();
   set_var_name( var, "Davenge" );
   set_var_to_string( var, new_string( "is awesome" ) );
   set_var_to_chunk( var, chunk );

   var = new_var();
   set_var_name( var, "Davenge2" );
   set_var_to_int( var, 28 );
   set_var_to_chunk( var, chunk );

   var = new_var();
   set_var_name( var, "davenge3" );
   set_var_to_int( var, 27 );
   set_var_to_chunk( var, chunk );

   var = new_var();
   set_var_name( var, "random" );
   set_var_to_string( var, new_string( "this is definitely random" ) );
   set_var_to_chunk( var, chunk );

   if( ( var = get_var_from_chunk( chunk, "Davenge" ) ) == NULL )
      bug( "%s: part 1 failed.", __FUNCTION__ );
   else
      bug( "%s: should be 'is awesome': %s", __FUNCTION__, get_var_string( var ) );


   if( ( var = get_var_from_chunk( chunk, "Davenge2" ) ) == NULL )
      bug( "%s: part 2 failed.", __FUNCTION__ );
   else
      bug( "%s: should be '28': %d", __FUNCTION__, get_var_int( var ) );

   if( ( var = get_var_from_chunk( chunk, "davenge3" ) ) == NULL )
      bug( "%s: part 3 failed.", __FUNCTION__ );
   else
      bug( "%s: should be '27': %d", __FUNCTION__, get_var_long( var ) );

   if( ( var = get_var_from_chunk( chunk, "random" ) ) == NULL )
      bug( "%s: part 4 failed.", __FUNCTION__ );
   else
      bug( "%s: should be 'this is definitely random': %s", __FUNCTION__, get_var_string( var ) );

   if( ( var = get_var_from_chunk( chunk, "anything" ) ) != NULL )
      bug( "%s: part 4 failed.", __FUNCTION__ );
   else
      bug( "%s: part 5 success.", __FUNCTION__ );
}

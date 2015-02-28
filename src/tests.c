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

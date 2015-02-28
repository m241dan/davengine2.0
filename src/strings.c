/*
 * This file handles string copy/search/comparison/etc.
 */
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>

/* include main header file */
#include "mud.h"

/*
 * Checks if aStr is a prefix of bStr.
 */
bool is_prefix(const char *aStr, const char *bStr)
{
  /* NULL strings never compares */
  if (aStr == NULL || bStr == NULL) return FALSE;

  /* empty strings never compares */
  if (aStr[0] == '\0' || bStr[0] == '\0') return FALSE;

  /* check if aStr is a prefix of bStr */
  while (*aStr)
  {
    if (tolower(*aStr++) != tolower(*bStr++))
      return FALSE;
  }

  /* success */
  return TRUE;
}

char *one_arg(char *fStr, char *bStr)
{
  /* skip leading spaces */
  while (isspace(*fStr))
    fStr++; 

  /* copy the beginning of the string */
  while (*fStr != '\0')
  {
    /* have we reached the end of the first word ? */
    if (*fStr == ' ')
    {
      fStr++;
      break;
    }

    /* copy one char */
    *bStr++ = *fStr++;
  }

  /* terminate string */
  *bStr = '\0';

  /* skip past any leftover spaces */
  while (isspace(*fStr))
    fStr++;

  /* return the leftovers */
  return fStr;
}

char *strdup(const char *s)
{
  char *pstr;
  int len;

  len = strlen(s) + 1;
  pstr = (char *) calloc(1, len);
  strcpy(pstr, s);

  return pstr;
}

int strcasecmp(const char *s1, const char *s2)
{
  int i = 0;

  while (s1[i] != '\0' && s2[i] != '\0' && toupper(s1[i]) == toupper(s2[i]))
    i++;

  /* if they matched, return 0 */
  if (s1[i] == '\0' && s2[i] == '\0')
    return 0;

  /* is s1 a prefix of s2? */
  if (s1[i] == '\0')
    return -110;

  /* is s2 a prefix of s1? */
  if (s2[i] == '\0')
    return 110;

  /* is s1 less than s2? */
  if (toupper(s1[i]) < toupper(s2[i]))
    return -1;

  /* s2 is less than s1 */
  return 1;
}

char *smash_color( const char *str )
{
   char *new_str, *ptr;

   if( !str )
      return NULL;

   new_str = str_alloc( get_size( str ) );
   ptr = new_str;

   for( ; *str != '\0'; str++ )
   {
      if( *str == '#' && *(str+1) != '\0' && *(str+1) != '#' )
         str++;
      else if( *(str+1) == '#' )
         continue;
      else
         *ptr++ = *str;
   }
   *ptr = '\0';
   return new_str;
}

int color_count( const char *str )
{
   int count = 0;
   if( !str )
      return 0;

   for( ; *str != '\0'; str++ )
      if( *str == '#' && *(str+1) != '#' )
         count++;

   return count;
}

int find_real_length( const char *str, int goal )
{
   int x, real;

   for( x = 0, real = 0; str[x] != '\0' && real < goal; )
   {
      if( str[x] == '#' )
      {
         /* hash at the end of a line */
         if( str[x+1] == '\0' )
            break;
         /* valid color tag */
         if( str[x+1] != '#' )
         {
            x += 2;
            continue;
         }
         else /* double hash */
         {
            x += 2;
            real++;
         }
      }
      x++;
      real++;
   }
   return x;
}

char *copy_string_fl( const char *orig, int length )
{
   char *new_str, *ptr;

   new_str = str_alloc( length + STRING_PADDING );
   ptr = new_str;
   for( int x = 0; x < length && *orig != '\0'; x++ )
      *ptr++ = *orig++;
   *ptr = '\0';
   return new_str;
}

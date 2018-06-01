
/**************************
 utils.c  1.0a  - 1998-07-03
            1.0b -  1999-12-29



***************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "utils.h"

int rem_sstring ( start, num, string )

int start ;		/* starting location of substring to delete */
int num ;		/* number of charachters to delete */
char string[] ;		/* string we are deleting the substring from */

{

int num_char ;

int index1 = start ;	    /* plats f–r f–rsta tecken som skall raderas	    */
int index2 = start + num;   /* plats f–r sista tecken  som skall raderas */

    /* kontrollera att start och antal tecken ”r giltiga */

if (( index1 < 0 ) | (index2 < 0) )
  return ( -1 ) ;

if ( index1 > index2 )
  return ( -1 ) ;

num_char = strlen ( string ) ;

if ( index1 > num_char )
  return ( -1 ) ;

if ( index2 > num_char )
  index2 = num_char ;		/* tag bort till slutet pÂ str”ngen */

while ( string[index1++] = string[index2++] )

  ;
return ( TRUE ) ;

}   /* of function remstring */


compare ( string ,sub )

/* j”mf–r f–rsta tecken */

char string[] ;
char sub [] ;

{

int i ;

for (i=0;i<(int)strlen(sub);i++) 
 {
  if ( toupper(string[i]) != toupper(sub[i]) )
	return ( 0 ) ;
 }

return ( 1 ) ;

}   /*    */


/*

Flytta till utils.c 

*/
int iindex ( char *string, char *sub ) 

{

int current = 0 ;

int j, k ;


while ( string[current] )
 {
  j = current ;
  k = 0 ;
    
  while ( string[j++] == sub[k] ) 
   if ( sub[++k] == (char) NULL )
     return ( current ) ;
  current ++ ;
 }
 
 return ( -1 ) ;  

}  /* end of iindex */ 

/* funktionen jämför en ISO-baserad datumsträng med dagens datum och tid
    och returnerar antal sekunder som skilje dessa datum åt  */


double difftoday_str ( char *timestr  )

{

char tmp[30] ;
char str[30] ;
double diff ;

time_t  today ;
time_t  yesterday ;
struct tm tp ;

char date [28] ;

time ( &today ) ;

strftime ( date, 28, "%Y-%m-%d %H:%M:%S", localtime(&today) ) ;

tmp[0] = (char) NULL ;
str[0] = (char) NULL ;
strcpy ( str, timestr ) ;

strncpy ( tmp, str, 4 ) ;                 /* YYYY */
tmp[4] = (char) NULL ;     
tp.tm_year = atoi ( tmp) - 1900 ;
rem_sstring ( 0, 5, str ) ;

strncpy ( tmp, str,2) ;                  /* MM */
tmp[2] = (char) NULL ;     
tp.tm_mon = atoi (tmp) -1 ;
rem_sstring (0, 3, str ) ;
  
strncpy ( tmp, str, 2 ) ;               /* DD */
tp.tm_mday = atoi ( tmp ) ;
rem_sstring (0,3, str ) ;

 strncpy ( tmp, str, 2 ) ;      /* hh */
tmp[2] = (char) NULL ;        
tp.tm_hour =  atoi ( tmp ) ;
rem_sstring (0,3, str ) ;

 strncpy ( tmp, str, 2 ) ;      /* mm */
tmp[2] = (char) NULL ;     
tp.tm_min = atoi ( tmp ) ;
rem_sstring (0,3, str ) ;

strncpy ( tmp, str, 2 ) ;     /* ss */
tmp[2] = (char) NULL ;     
tp.tm_sec = atoi ( tmp ) ;
rem_sstring ( 0,2, str ) ;  
tp.tm_isdst = -1 ;

yesterday = mktime ( &tp ) ;    /* föregående tid */


diff = difftime ( today, yesterday ) ; 

return ( diff  ) ;

}


int GetLocaleText ( char *filename, int locale, int id, char *text )


{
	
char temp[255];
char string[512] ;
int i,j ;

FILE *fp ;

text[0] = (char) NULL ;

if (( fp = fopen ( filename, "r" )) == NULL )
   {
    printf ( "Can't open file %s.\n", filename ) ;
    exit ( 1 ) ;
   }

while ( fgets ( string, 255, fp ) != NULL )
   {
 
      /* kolla om rätt locale */

  for (i=0;i<(int)strlen(string);i++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[i] = string[i] ;
   }
  temp[i] = (char) NULL ;

       /* om inte rätt locale så fortsätt till nästa rad */
  if  ( locale != atoi ( temp )) 
    continue ;
  i++ ;

    /* id */
  
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;
  
     /* om inte rätt id så gå till nästa rad */

  if  ( id != atoi ( temp ) )
    continue ;
  i++ ;
  
  /* läs in anpassad text för ett land */
  
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    text[j] = string[i] ;
   }

  text[j] = (char) NULL ;
  return ( TRUE ) ;
  
 }

/* plocka bort alla tecken efter nyrad */
for (;j>=0;j--)
  if ( text[j] == '\n' )
    text[j] = (char) NULL ;

fclose ( fp ) ;

return ( FALSE ) ;

}

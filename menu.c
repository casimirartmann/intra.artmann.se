/********************************************************

 Konferanssystem 

 program:  menu.c
 version:  2.1.2.4
 date:     01/02/24
 
 Visar menyval för konferanssystem


*********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SELECT 0
#define INSERT 1
#define UPDATE 2
#define DELETE 3
#define EDIT 4
#define LIST 5

#define TRUE 1
#define FALSE 0 

#define BGCOLOR "#003FDF"
#define LINK "white"
#define VLINK "white"
#define ALINK "yellow"
#define TEXT "black "
  
  /* funktioner för hantering av cgi-anrop och felhantering */
  
#include "../cgi.h"
#include "../utils.h"

  /* gemensamma funktioner för konferanssystemet */
  
#include "intra.h"
#include "db.h"  
#include "html.h"

  /* följande funktioner anropas  */
     
  /* expandMethod
     expandNewsgroup

     readSubject               - hämta en eller flera rubriker 
     readNewsgroup             - hämta en eller fler namn på nyhetsgrupper
      
     printConferanceMenuHTML   - skriv ut tillgängliga menyval
     printNewsgroupHTML        - skriv ut valda nyhetsgrupper
     printSubjectHTML          - skriv ut valda rubriker
  */
 
static char sccsid[] = " " ;

struct _menu { int id ;
               char name[32] ;
               char script[64] ;
               char value[128] ;
             }  ;

void printMenuHTML ( int *no ) ;

int ExpandMenu ( struct _CGIarg *CGIarg, int *no, int *subno ) ;

 /* huvudprogram  menu */

main ( int argc, char *argv[] )



{

int method ;
int no ;                   /* meny */
int subno ;                /* submenu */
char path[255] ;

char *instring ;             /* inläst teckensträng från formulär */
struct _CGI CGI ;            /* CGI environment */
struct _CGIarg CGIarg ;      /* lista med varibelnamn och värden från formulär*/


  /* läs in CGI environment */

strcpy ( path, argv[0] ) ;
InitIntra ( path ) ;

InitCGI () ;
GetCGI ( &instring, &CGI ) ;      

  /* konvertera värden från formulär */

bURLencode ( instring, &CGIarg ) ;
ExpandMethod ( &CGIarg, &method ) ;
ExpandMenu ( &CGIarg, &no, &subno) ;  

  /* hämta information om användaren
     baserat på inloggning  via WWW-server */

  /* visa alla rubriker är en grupp */
                       
if ( method == SELECT )
 {    
  if ( no >= 0 )                   
    printMenuHTML ( &no ) ;
                                   
 }

return ( TRUE ) ;                    


}  /* end of main () */


void printMenuHTML ( int *no ) 

{
FILE *fp ;
int i,j ;
int counter ;    /* antal menyval som skrivs ut */

char temp[16] ;
char string [512 ] ; 
char filename [ 128] ; 

struct _menu menu ;               

sprintf ( filename, "standard.mnu" ) ;

printf("<HTML>") ;
printf("<BODY bgcolor=\"%s\" text=\"%s\" link=\"%s\" vlink=\"%s\" alink=\"%s\">\n", BGCOLOR, TEXT, LINK, VLINK, ALINK ) ;

if (( fp = fopen ( filename, "r" )) == NULL )
   {
    errorHTML ("Kan inte läsa menyfilen") ;
    return  ;
   }

counter = 0 ;

while ( fgets ( string, 256, fp ) != NULL )
 {
  /* plocka fram id för varje menyval */ 
   
  for (i=0;i<(int)strlen(string);i++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[i] = string[i] ;
   }
  temp[i] = (char) NULL ;
  menu.id = atoi ( temp) ;
  i++ ;                    
    
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    menu.name[j] = string[i] ;
   }
  menu.name[j] = (char) NULL ;
  i++ ;  

  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    menu.script[j] = string[i] ;
   }
  menu.script[j] = (char) NULL ;
  i ++ ;

  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    menu.value[j] = string[i] ;
   }
  menu.value[j] = (char) NULL ;

  if ( menu.id == 0 )  /* continue with main menu until submenu or main menu finished */
    {
      printf("<a href=\"%s/%s%s?%s\" target=\"menu\">%s</a><br><p>\n", CGI_PATH, menu.script, EXECUTABLE, menu.value, menu.name ) ;
      counter ++ ;
    }

   
  if (( *no != 0 ) && ( menu.id == 0 ) && ( counter == *no ))
   break ;
  }

fclose ( fp ) ;

if ( *no > 0 ) /* print submenu in a smaller font */
   {


if (( fp = fopen ( filename, "r" )) == NULL )
   {
    errorHTML ("Kan inte läsa menyfilen") ;
    return  ;
   }

counter = 0 ;

while ( fgets ( string, 256, fp ) != NULL )
 {
  /* plocka fram id för varje menyval */ 
   
  for (i=0;i<(int)strlen(string);i++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[i] = string[i] ;
   }
  temp[i] = (char) NULL ;
  menu.id = atoi ( temp) ;
  i++ ;                    
    
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    menu.name[j] = string[i] ;
   }
  menu.name[j] = (char) NULL ;
  i++ ;  

  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    menu.script[j] = string[i] ;
   }
  menu.script[j] = (char) NULL ;
  i ++ ;

  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    menu.value[j] = string[i] ;
   }
  menu.value[j] = (char) NULL ;

    if ( menu.id == *no )  /* continue with main menu until submenu or main menu finished */
     {
      printf("<font size=-1>") ;
      printf("- <a href=\"%s/%s%s?%s\" target=\"desk\">%s</a><br>\n", CGI_PATH, menu.script, EXECUTABLE, menu.value, menu.name ) ;
      printf ("</font>"); }  

     counter ++ ;

     if ( menu.id > *no ) 
       break ;
      }

fclose ( fp ) ;

if (( fp = fopen ( filename, "r" )) == NULL )
   {
    errorHTML ("Kan inte läsa menyfilen") ;
    return  ;
   }

counter = 0 ;

printf("<p>") ;

while ( fgets ( string, 256, fp ) != NULL )
 {
  /* plocka fram id för varje menyval */ 
   
  for (i=0;i<(int)strlen(string);i++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[i] = string[i] ;
   }
  temp[i] = (char) NULL ;
  menu.id = atoi ( temp) ;
  i++ ;                    
    
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    menu.name[j] = string[i] ;
   }
  menu.name[j] = (char) NULL ;
  i++ ;  

  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    menu.script[j] = string[i] ;
   }
  menu.script[j] = (char) NULL ;
  i ++ ;

  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    menu.value[j] = string[i] ;
   }
  menu.value[j] = (char) NULL ;

  if ( menu.id == 0 )  /* continue with main menu until submenu or main menu finished */
    {
    if ( counter >= *no  )
      printf("<a href=\"%s/%s%s?%s\" target=\"menu\">%s</a><br><p>\n", CGI_PATH, menu.script, EXECUTABLE, menu.value, menu.name ) ;
  counter ++ ;
    } 

  if ( menu.id > 0 ) 
   break ;
  }

fclose ( fp ) ;

 } 

printf("</BODY></HTML>\n") ;  
  
}  /* end of printMenuHTML ( int no )*/ 

int ExpandMenu( struct _CGIarg *CGIarg, int *no, int *subno ) 

{

struct _CGIarg *first ;

  /* spara en pekare till första argumentet i listan */ 
  
first = malloc ( sizeof( *CGIarg )) ; 
first = CGIarg ; 

*no = -1 ;
*subno = -1 ;

while ( CGIarg != NULL )
 {
  if ( !strncmp("menu", CGIarg->name, 4 ))
   {
    *no = atoi(CGIarg->value ) ;
   }
    
  if ( !strncmp("submenu", CGIarg->name, 7 ))
   {
    *subno = atoi(CGIarg->value ) ;
   }
    
  CGIarg = CGIarg->next ;
 }

CGIarg = first ;

/*free ( first ) ;*/

if ( *no != -1 ) 
  return ( TRUE ) ;
else
  return ( FALSE ) ;
    
}   /* end of menu */

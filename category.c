/***************************************************************************
 Konferanssystem
 Program: %M%
 Version: %I%
 Date:    2006-08-24
     written by Casimir Artmann   
 (c) copyright Casimir Artmann. 1996-1998.
 ****************************************************************************/ 

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


  /* funktioner för hantering av cgi-anrop och felhantering */

  
#include "../cgi.h"
#include "../utils.h"


  /* gemensamma funktioner för konferanssystemet */
  
#include "intra.h"
#include "db.h"  
#include "html.h"

  /* följande funktioner anropas  */
    

  /*  expandMethod              - avkoda vad som skall göras
      expandCategory           - avkoda vad som skall visas
      readCategory             - hämta en eller flera rubriker 
      printCategoryHTML          - skriv ut valda rubriker

  */

 
int ExpandCategory ( struct _CGIarg *CGIarg, struct _category *category ) ;

 /* huvudprogram för conferance.c */

main (  argc, argv)

int argc ;

char *argv[ ] ; 

{

char path[255] ;

int method ;                     /* menyval för programmet */

char *instring ;
struct _CGI CGI ;
struct _CGIarg CGIarg ; 

struct _category  category ;    /* länkad lista katagorier */

struct _user user ;

  /* läs in CGI environment */

strcpy ( path, argv[0] ) ;
InitIntra ( path ) ; 

InitCGI () ;
GetCGI ( &instring, &CGI ) ;   

  /* konvertera värden från formulär */

bURLencode ( instring, &CGIarg ) ;
ExpandMethod ( &CGIarg, &method ) ;

ExpandCategory ( &CGIarg, &category ) ; 
findUser ( CGI.remote_user, &user ) ;
                 
if ( method == SELECT )            /* visa alla katagorier */
  {                     
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny för konferanssystem */
   if ( !FRAMES ) 
     exit(0) ; 

   selectCategory ( &category,"ALL","", NULL ) ; /* läs önskade nyhetsgrupper från fil */
   printCategoryHTML ( user, &category ) ;                 /* skriv ut nyhetsgrupper som HTML-kod    */

   catHTML ( FOOTER ) ;           /* skriv ut en sidfot i HTML        */
   return ( TRUE ) ;    
  }

if ( method == INSERT )
  {
    catHTML ( HEADER ) ;

    if ( user.id == -1 )
      exit(0) ;   
              
    insertCategory ( &category ) ;
    selectCategory ( &category,"ALL","", NULL ) ; /* läs önskade nyhetsgrupper från fil */

    printCategoryHTML ( user, &category ) ;                 /* skriv ut nyhetsgrupper som HTML-kod    */

    catHTML ( FOOTER ) ;
	return ( TRUE ) ;     
  }


if ( method == EDIT ) 
  {
    catHTML ( HEADER ) ;

    if ( user.id == -1 )
      exit(0) ;   

    printCategoryFormHTML ( category ) ;     

    catHTML ( FOOTER ) ; 
	return ( TRUE ) ;    
  }

if ( method == UPDATE ) ;
  {
    catHTML ( HEADER ) ;

    if ( user.id == -1 )
      exit(0) ;   
 
    updateCategory ( &category ) ;
    selectCategory ( &category,"ALL","", NULL ) ; /* läs önskade nyhetsgrupper från fil */

    printCategoryHTML ( user, &category ) ;                 /* skriv ut nyhetsgrupper som HTML-kod    */

    catHTML ( FOOTER ) ; 
	return ( TRUE ) ;    
  }

if ( method == DELETE ) 
  {
    catHTML ( HEADER ) ;
  
    if ( user.id == -1 )
      exit(0) ;   
               
    deleteCategory ( &category ) ;
    selectCategory ( &category,"ALL","", NULL ) ; /* läs önskade nyhetsgrupper från fil */

    printCategoryHTML ( user, &category ) ;                 /* skriv ut nyhetsgrupper som HTML-kod    */

    catHTML ( FOOTER ) ; 
	return ( TRUE ) ;    
	
  }

return ( FALSE) ;                    

}  /* end of main () */


int ExpandCategory ( struct _CGIarg *CGIarg, struct _category *category ) 

  /* 

      Funktionen översätter argument från CGI-script till en post i en länkad lista
 
      Om inga fel inträffar returneras TRUE

      Om värde överskrider min eller maxvärde så returnerar funktionen FALSE

      Om någon annan typ av fel inträffar så returneras ERROR

      CGIarg innhåller en länkad lista med variabelnamn och värden

      

      *subject innehåller en pekare till en post med värden. 

      Om värden saknas sätts NULL 
  
      Följande argument får från formulär

      id=      1...MAXINT  
      name=    0 - 64 tecken
      remark=  0 - 255 tecken
      public=  on,-1, 1...MAXINT
      owner=   on,-1, 1...MAXINT

 */

{

struct _CGIarg *first ;

   /* nollställ alla värden */
   
category->id = -1 ;
category->owner = -1 ;
category->security = -1 ;
category->created[0] = (char) NULL ;
category->name[0] = (char) NULL ;
category->remark[0] = (char) NULL ;


   /* reservera minne för pekare till första posten */

first = malloc ( sizeof ( *CGIarg )) ;

if ( first == NULL )                      /* */
 exit ( -1 ) ;                        

first = CGIarg ; 
   /* sök igenom parametrar för att ta emot värde */

while ( CGIarg != NULL ) 
 {

  if ( !strncmp("id", CGIarg->name, 2 ))
    category->id = atoi ( CGIarg->value ) ;
	
   if ( !strncmp("group", CGIarg->name, 5 ))
    category->group = atoi ( CGIarg->value ) ;

  if ( !strncmp("name", CGIarg->name, 4 ))
    strncpy ( category->name, CGIarg->value, 64 ) ;

  if ( !strncmp("remark", CGIarg->name, 6 ))
    strncpy ( category->remark, CGIarg->value, 255 ) ;

  if ( !strncmp("public", CGIarg->name, 6 ))
    if ( !strcmp ( "on", CGIarg->value ))
       category->security = -1 ;        
    else
       category->security = atoi ( CGIarg->value ) ;
	   
  if ( !strncmp("owner", CGIarg->name, 5 ))
    if ( !strcmp ( "on", CGIarg->value ))
       category->owner = -1 ;
    else
       category->owner = atoi ( CGIarg->value ) ;

  CGIarg = CGIarg->next ;

 }

CGIarg = first ;

/* free ( first ) ; */ /* Bortkommenterat för linux */

return ( TRUE ) ;

}  /* end of function */

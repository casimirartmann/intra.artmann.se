
 /***************************************************************************
 
 Konferanssystem version 2.1

 program:  subject.c
 version:   01/02/10
 date:        2.1.2.4
  
  written by Casimir Artmann
  
  (c) copyright Casimir Artmann. 1996-1998.


 
 ****************************************************************************/ 
 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static char sccsid[] = "@(#)subject.c	2.1.2.4 01/02/10" ;

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
     
  /* expandMethod              - avkoda vad som skall göras
     expandSubject             - avkoda vad som skall visas
      
  */
 

 /* huvudprogram för subject.c */

main ( int argc, char *argv[])


{

char path[255] ;                /* sökväg inkl programnamn */
int method ;                     /* menyval för programmet */
int status ;                     /* returvärde från funktion för koll av resultat */
char *instring ;
struct _CGI CGI ;
struct _CGIarg CGIarg ; 

struct _user user ;                         /* användarinformation */
struct _subject subject ;    /* länkad lista för nyhetsgrupper */
struct _category category ;           /* länkad lista för kategorier */

char sLocale[255] ;
subject.category = 0 ;
subject.id = 0 ;
subject.name[0] = (char) NULL ;
subject.remark[0] = (char) NULL ;
strcpy ( path, argv[0] ) ;
InitIntra ( path ) ; 

  /* läs in CGI environment */
InitCGI () ;
GetCGI ( &instring, &CGI ) ;   
  
  /* konvertera värden från formulär */

bURLencode ( instring, &CGIarg ) ;
ExpandMethod ( &CGIarg, &method ) ;

ExpandSubject ( &CGIarg, &subject ) ;

/* hämta information om användaren
    baserat på inloggning  via WWW-server */

findUser ( CGI.remote_user, &user ) ;
                       
if ( method == SELECT )            /* visa alla nyhetsgrupper */
 {                     
  catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny för konferanssystem */
  if ( !FRAMES ) 
    exit (0) ;

  selectSubject ( &subject,"ALL","DATE", NULL ) ; /* läs önskade nyhetsgrupper från fil */
  printSubjectHTML ( user, &subject ) ; /* skriv ut nyhetsgrupper som HTML-kod    */

  catHTML ( FOOTER ) ;           /* skriv ut en sidfot i HTML        */
 }

if ( method == INSERT )            /* lägg till en nyhetsgrupp */
 { 
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny för konferanssystem */

   if (!FRAMES )
    exit(0) ;

   status = insertSubject ( &subject ) ; /* spara nyhetsgruppen */

   /* om gruppen sparades skall aktuella grupper listas  
      om en grupp med samma namn finns sedan tidigare skall
      den nya gruppen inte sparas, utan ett felmeddelande skrivs ut
      detsamma gäller om det blir fel vid skrivning till databasen */ 
   
   if ( status )
     {      /* läs önskade nyhetsgrupper från fil */
      selectSubject ( &subject,"ALL","DATE", NULL ) ; 
      printSubjectHTML ( user, &subject ) ; /* skriv ut nyhetsgrupper som HTML-kod    */
     }
   catHTML ( FOOTER ) ;           /* skriv ut en sidfot i HTML        */
 }

if ( method == DELETE ) 
 {
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny för konferanssystem */
   if ( !FRAMES)
    exit(0) ;

   deleteSubject ( &subject ) ;      /* radera ett ämne  */

   catHTML ( FOOTER ) ;           /* skriv ut en sidfot i HTML        */
 }

if ( method == EDIT ) 
 {
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny för konferanssystem */
   if (!FRAMES)
     exit(0) ;

   if ( !strcmp ( EDITION, "Limited"))                 /* Limited edition */
    printSubjectFormHTML ( subject );
   else
     if ( subject.category > 0 )                          /* standard edition */
       printSubjectFormHTML ( subject ) ;
     else
	 {
 
      GetLocaleText ( MESSAGES, LOCALE, 54, sLocale ) ; /* Category for subject */
	  category.id = subject.category ;
      printObjectCategoryForm ( category, "subject", sLocale ) ;

     }

   catHTML ( FOOTER ) ;           /* skriv ut en sidfot i HTML        */
 }

  /* allokerat minne för nyhetsgrupper släpps ej fri i koden */ 
  
return ( TRUE ) ;                    


}  /* end of main () */


int ExpandSubject ( struct _CGIarg *CGIarg, struct _subject *subject ) 

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
    
subject->category = -1 ;
subject->id = -1 ;
subject->owner = -1 ;
subject->security = -1 ;
subject->article = -1 ;
subject->created[0] = (char) NULL ;
subject->name[0] = (char) NULL ;
subject->remark[0] = (char) NULL ;
   
   /* reservera minne för pekare till första posten */
   
first = malloc ( sizeof ( *CGIarg )) ;
if ( first == NULL )                      /* */
 exit ( -1 ) ;                        
 
first = CGIarg ; 

   /* sök igenom parametrar för att ta emot värde */
   
while ( CGIarg != NULL ) 
 {
  { if ( !strncmp("category", CGIarg->name, 8 ))
    subject->category = atoi ( CGIarg->value ) ;
    }
  if ( !strncmp("id", CGIarg->name, 2 ))
    subject->id = atoi ( CGIarg->value ) ;
    
  if ( !strncmp("name", CGIarg->name, 4 ))
    strncpy ( subject->name, CGIarg->value, 64 ) ;

  if ( !strncmp("remark", CGIarg->name, 6 ))
    strncpy ( subject->remark, CGIarg->value, 255 ) ;

  if ( !strncmp("public", CGIarg->name, 6 ))
    if ( !strcmp ( "on", CGIarg->value ))
       subject->security = -1 ;        
    else
       subject->security = atoi ( CGIarg->value ) ;

  if ( !strncmp("owner", CGIarg->name, 5 ))
    if ( !strcmp ( "on", CGIarg->value ))
       subject->owner = -1 ;
    else
       subject->owner = atoi ( CGIarg->value ) ;
    
  CGIarg = CGIarg->next ;
 }

CGIarg = first ;
  /* free ( first ) ; */

return ( TRUE ) ;


}  /* end of function ExpandSubject */


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
  
  /* funktioner f�r hantering av cgi-anrop och felhantering */
  
#include "../cgi.h"
#include "../utils.h"

  /* gemensamma funktioner f�r konferanssystemet */
  
#include "intra.h"
#include "db.h"  
#include "html.h"

  /* f�ljande funktioner anropas  */
     
  /* expandMethod              - avkoda vad som skall g�ras
     expandSubject             - avkoda vad som skall visas
      
  */
 

 /* huvudprogram f�r subject.c */

main ( int argc, char *argv[])


{

char path[255] ;                /* s�kv�g inkl programnamn */
int method ;                     /* menyval f�r programmet */
int status ;                     /* returv�rde fr�n funktion f�r koll av resultat */
char *instring ;
struct _CGI CGI ;
struct _CGIarg CGIarg ; 

struct _user user ;                         /* anv�ndarinformation */
struct _subject subject ;    /* l�nkad lista f�r nyhetsgrupper */
struct _category category ;           /* l�nkad lista f�r kategorier */

char sLocale[255] ;
subject.category = 0 ;
subject.id = 0 ;
subject.name[0] = (char) NULL ;
subject.remark[0] = (char) NULL ;
strcpy ( path, argv[0] ) ;
InitIntra ( path ) ; 

  /* l�s in CGI environment */
InitCGI () ;
GetCGI ( &instring, &CGI ) ;   
  
  /* konvertera v�rden fr�n formul�r */

bURLencode ( instring, &CGIarg ) ;
ExpandMethod ( &CGIarg, &method ) ;

ExpandSubject ( &CGIarg, &subject ) ;

/* h�mta information om anv�ndaren
    baserat p� inloggning  via WWW-server */

findUser ( CGI.remote_user, &user ) ;
                       
if ( method == SELECT )            /* visa alla nyhetsgrupper */
 {                     
  catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny f�r konferanssystem */
  if ( !FRAMES ) 
    exit (0) ;

  selectSubject ( &subject,"ALL","DATE", NULL ) ; /* l�s �nskade nyhetsgrupper fr�n fil */
  printSubjectHTML ( user, &subject ) ; /* skriv ut nyhetsgrupper som HTML-kod    */

  catHTML ( FOOTER ) ;           /* skriv ut en sidfot i HTML        */
 }

if ( method == INSERT )            /* l�gg till en nyhetsgrupp */
 { 
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny f�r konferanssystem */

   if (!FRAMES )
    exit(0) ;

   status = insertSubject ( &subject ) ; /* spara nyhetsgruppen */

   /* om gruppen sparades skall aktuella grupper listas  
      om en grupp med samma namn finns sedan tidigare skall
      den nya gruppen inte sparas, utan ett felmeddelande skrivs ut
      detsamma g�ller om det blir fel vid skrivning till databasen */ 
   
   if ( status )
     {      /* l�s �nskade nyhetsgrupper fr�n fil */
      selectSubject ( &subject,"ALL","DATE", NULL ) ; 
      printSubjectHTML ( user, &subject ) ; /* skriv ut nyhetsgrupper som HTML-kod    */
     }
   catHTML ( FOOTER ) ;           /* skriv ut en sidfot i HTML        */
 }

if ( method == DELETE ) 
 {
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny f�r konferanssystem */
   if ( !FRAMES)
    exit(0) ;

   deleteSubject ( &subject ) ;      /* radera ett �mne  */

   catHTML ( FOOTER ) ;           /* skriv ut en sidfot i HTML        */
 }

if ( method == EDIT ) 
 {
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny f�r konferanssystem */
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

  /* allokerat minne f�r nyhetsgrupper sl�pps ej fri i koden */ 
  
return ( TRUE ) ;                    


}  /* end of main () */


int ExpandSubject ( struct _CGIarg *CGIarg, struct _subject *subject ) 

  /* 
      Funktionen �vers�tter argument fr�n CGI-script till en post i en l�nkad lista
      
      Om inga fel intr�ffar returneras TRUE
      Om v�rde �verskrider min eller maxv�rde s� returnerar funktionen FALSE
      Om n�gon annan typ av fel intr�ffar s� returneras ERROR
      
      CGIarg innh�ller en l�nkad lista med variabelnamn och v�rden
      
      *subject inneh�ller en pekare till en post med v�rden. 
      Om v�rden saknas s�tts NULL 
      
      F�ljande argument f�r fr�n formul�r
       
      id=      1...MAXINT  
      name=    0 - 64 tecken
      remark=  0 - 255 tecken
      public=  on,-1, 1...MAXINT
      owner=   on,-1, 1...MAXINT

 */

{

struct _CGIarg *first ;

   /* nollst�ll alla v�rden */
    
subject->category = -1 ;
subject->id = -1 ;
subject->owner = -1 ;
subject->security = -1 ;
subject->article = -1 ;
subject->created[0] = (char) NULL ;
subject->name[0] = (char) NULL ;
subject->remark[0] = (char) NULL ;
   
   /* reservera minne f�r pekare till f�rsta posten */
   
first = malloc ( sizeof ( *CGIarg )) ;
if ( first == NULL )                      /* */
 exit ( -1 ) ;                        
 
first = CGIarg ; 

   /* s�k igenom parametrar f�r att ta emot v�rde */
   
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

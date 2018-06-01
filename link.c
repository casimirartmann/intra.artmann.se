
/********************************************************

 Konferanssystem

 Program: %M%
 Version:  %I%
 Date:       %D%

 Programmet hanterar l�nkar  i ett konferanssystem

 written by Casimir Artmann.
 Copyright Casimir Artmann. 1996-1998.
 
*********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


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
     
  /* expandMethod
     expandlink
  
     savelink               - spara artikel
     readlink               - h�mta en artikel
     readSubject               - h�mta en eller flera rubriker 
     readNewsgroup             - h�mta en eller fler namn p� nyhetsgrupper

     printConferanceMenuHTML   - skriv ut tillg�ngliga menyval
     printNewsgroupHTML        - skriv ut valda nyhetsgrupper
     printSubjectHTML          - skriv ut valda rubriker
     printlinkHTML          - skriv ut en artikel
     printlinkForm          - skriv ut ett HTML-formul�r f�r skriva en artikel         
  */
  
 /* huvudprogram link */

main ( int argc, char *argv[] )


{

char path[255] ;
int method ;                   /* vad skall programmet utf�ra */
char *instring ;               /* inl�st teckenstr�ng */

struct _CGI CGI ; 
struct _CGIarg CGIarg ; 

struct _user user ;            /* information om anv�ndaren */
/*struct _title *title ;      /* rubriker f�r nyhetsgruppen */
struct _category category ;  /* kategori f�r inl�gget */
struct _link link ;

char created [26] ;
char filename[128] ; 

time_t today ;
FILE *logfile ;

link.indent = 0 ;
link.category = 0 ;
link.subject = 0 ;
link.id = 0 ;
link.group = -1 ;
link.owner = -1 ;
link.title[0] = (char) NULL ;
link.author[0] = (char) NULL ;
link.email[0] = (char) NULL ;
link.href[0] = (char) NULL ;
link.company[0] = (char) NULL ;    
link.created[0] = (char) NULL ;

  /* l�s in CGI environment */
strcpy ( path, argv[0] ) ;
InitIntra ( path ) ;

InitCGI () ;
GetCGI ( &instring, &CGI ) ;     

  /* konvertera v�rden fr�n formul�r */

bURLencode ( instring, &CGIarg ) ;
ExpandMethod ( &CGIarg, &method ) ;
ExpandLink ( &CGIarg, &link) ;

  /* h�mta information om anv�ndaren
    baserat p� inloggning  via WWW-server */

   findUser ( CGI.remote_user, &user ) ;

if (( logfile = fopen ( "/usr/local/apache/db/intra.log", "a" )) == NULL )
   exit ( -1 ) ;
	
 time( &today ) ;
 strftime( created, 26,"%Y-%m-%d %H:%M:%S", localtime(&today) ) ;
 fprintf (logfile, "%s %s %s %s %d %d %d %d\n", created, CGI.remote_user, CGI.remote_addr, CGI.remote_host, method, link.category, link.subject, link.id ) ;
 fclose ( logfile ) ;


  /* visa ett inl�gg */
              
if ( method ==  SELECT )
  {
  
   selectLink ( link.category, link.subject, link.id, &link ) ;

   /* l�nka vidare */

   printLinkHTML ( link ) ;
   
}

if ( method ==  EDIT )
  {
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny f�r konferanssystem */
   if ( link.id != 0 )    /* befintligt inl�gg skall �ndras  */
    {
     selectLink ( link.category, link.subject, link.id, &link ) ;   
    }
 
   if ( user.id > 0 )   /* om inte inloggad anv�ndare nollst�ll namn */
   {
      strcpy ( link.author, user.name ) ;
      strcpy ( link.email, user.epost ) ;
      link.owner = user.id ;
     }
 
  if ( strcmp ( EDITION, "Limited"))
    {
      if ( link.category > 0  )
        printLinkForm ( link ) ; /* skriv ut ett formul�r f�r ett nytt meddelande */
     else
	 { 
	  category.id = link.category  ;
       printLinkCategoryForm ( category ) ;    /* v�lj vilken katagori som avses */
	 }
     }
  else
     printLinkForm ( link ) ; /* skriv ut ett formul�r f�r ett nytt meddelande */

    catHTML ( FOOTER ) ;           /* skriv ut en sidfot i HTML        */
  }

  /* l�gg till ett nytt inl�gg */
                       
if ( method == INSERT )
  {
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */

   /* skriv ut meny f�r konferanssystem */
 
  if (!FRAMES ) 
   exit(0) ;
  
  if ( user.id == -1 )          /* det g�r inte att �ndra ett inl�gg om man inte �r inloggad */ 
     exit (0) ;              
  
  link.owner = user.id ;
  insertLink ( &link ) ;     /* spara en ny l�nk */

  catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
 
  printLinkHTML ( link ) ;
 
  }

  /* visa ett inl�gg */
                       
if ( method == DELETE )
  {
   catHTML ( HEADER ) ;          /* skriv ut ett sidhuvud i HTML     */
     if ( !FRAMES )
     exit(0) ;                   /* skriv ut meny f�r konferanssystem */

   if ( user.id == -1 )          /* det g�r inte att �ndra ett inl�gg om man inte �r inloggad */ 
     exit (0) ;                

   deleteLink ( &link ) ;        /* radera l�nk  */
   catHTML ( HEADER ) ;          /* skriv ut ett sidhuvud i HTML     */
   exit ( FALSE ) ;
  }

if ( method == UPDATE )
  {
 
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
  
   if ( user.id == -1 )          /* det g�r inte att �ndra ett inl�gg om man inte �r inloggad */ 
     exit (0) ;              

   updateLink( &link ) ;     /* spara l�nk */
     
   printLinkHTML ( link ) ; /* skriv ut artikel som HTML-kod    */
  
   /*   groupMail ( &link ) ;   */

   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
   exit ( TRUE ) ;
  
  }


return ( TRUE ) ;                     

}  /* end of main () */

int ExpandLink ( struct _CGIarg *CGIarg, struct _link *link ) 

  /* 
      Funktionen �vers�tter argument fr�n CGI-script till en post i en l�nkad lista
      
      Om inga fel intr�ffar returneras TRUE
      Om v�rde �verskrider min eller maxv�rde s� returnerar funktionen FALSE
      Om n�gon annan typ av fel intr�ffar s� returneras ERROR
      
      CGIarg innh�ller en l�nkad lista med variabelnamn och v�rden
      
      *article inneh�ller en pekare till en post med v�rden. 
      Om v�rden saknas s�tts NULL     

      F�ljande argument fr�n formul�r anv�nds av funktionen
       
      id=        
      group=
      title=
      reply=
      comment=
      author=
      email=
      to=
      company=
      text=    
  */
  

{

  int size ;                           /* antal tecken som varialbel inneh�ller */
struct _CGIarg *first ;          

first = malloc ( sizeof ( *CGIarg )) ;
first = CGIarg ; 

while ( CGIarg != NULL ) 
 {

  size = strlen ( CGIarg->value ) ;

  if ( !strncmp("category", CGIarg->name, 8 )) {
    link->category = atoi ( CGIarg->value ) ;
   }
  if ( !strncmp("id", CGIarg->name, 2 )) {
    link->id = atoi ( CGIarg->value ) ;
   }
  if ( !strncmp("subject", CGIarg->name, 7 )) {
    link->subject = atoi ( CGIarg->value ) ;
   }
 if ( !strncmp("group", CGIarg->name, 5 )) {
    link->group = atoi ( CGIarg->value ) ;
   }

  if ( !strncmp("indent", CGIarg->name, 6 )) {
    link->indent = atoi ( CGIarg->value ) ;
   }
  if ( !strncmp("title", CGIarg->name, 5 )) {
    strncpy( link->title, CGIarg->value, 64 ) ;
    link->title[size] = (char) NULL ;
   }

   if ( !strncmp("author", CGIarg->name, 6 )) {
    strncpy( link->author, CGIarg->value,64 ) ;
   link->author[size] = (char) NULL ;
   }

   if ( !strncmp("email", CGIarg->name, 4 )) {
    strncpy( link->email, CGIarg->value,128 ) ;
   link->email[size] = (char) NULL ;

   }
   if ( !strncmp("link", CGIarg->name, 4 )) {
    strncpy( link->href, CGIarg->value,128 ) ;
   link->href[size] = (char) NULL ;
   }
   if ( !strncmp("company", CGIarg->name, 7 )) {
    strncpy( link->company, CGIarg->value, 64 ) ;
   link->company[size] = (char) NULL ;
   }
 
  CGIarg = CGIarg->next ;
 }

CGIarg = first ;
/*free ( first ) ;*/

return ( TRUE ) ;

}   /* end of ExpandLink */
                       


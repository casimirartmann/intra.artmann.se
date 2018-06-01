
/********************************************************

 Konferanssystem

 Program: %M%
 Version:  %I%
 Date:       %D%

 Programmet hanterar länkar  i ett konferanssystem

 written by Casimir Artmann.
 Copyright Casimir Artmann. 1996-1998.
 
*********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


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
     
  /* expandMethod
     expandlink
  
     savelink               - spara artikel
     readlink               - hämta en artikel
     readSubject               - hämta en eller flera rubriker 
     readNewsgroup             - hämta en eller fler namn på nyhetsgrupper

     printConferanceMenuHTML   - skriv ut tillgängliga menyval
     printNewsgroupHTML        - skriv ut valda nyhetsgrupper
     printSubjectHTML          - skriv ut valda rubriker
     printlinkHTML          - skriv ut en artikel
     printlinkForm          - skriv ut ett HTML-formulär för skriva en artikel         
  */
  
 /* huvudprogram link */

main ( int argc, char *argv[] )


{

char path[255] ;
int method ;                   /* vad skall programmet utföra */
char *instring ;               /* inläst teckensträng */

struct _CGI CGI ; 
struct _CGIarg CGIarg ; 

struct _user user ;            /* information om användaren */
/*struct _title *title ;      /* rubriker för nyhetsgruppen */
struct _category category ;  /* kategori för inlägget */
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

  /* läs in CGI environment */
strcpy ( path, argv[0] ) ;
InitIntra ( path ) ;

InitCGI () ;
GetCGI ( &instring, &CGI ) ;     

  /* konvertera värden från formulär */

bURLencode ( instring, &CGIarg ) ;
ExpandMethod ( &CGIarg, &method ) ;
ExpandLink ( &CGIarg, &link) ;

  /* hämta information om användaren
    baserat på inloggning  via WWW-server */

   findUser ( CGI.remote_user, &user ) ;

if (( logfile = fopen ( "/usr/local/apache/db/intra.log", "a" )) == NULL )
   exit ( -1 ) ;
	
 time( &today ) ;
 strftime( created, 26,"%Y-%m-%d %H:%M:%S", localtime(&today) ) ;
 fprintf (logfile, "%s %s %s %s %d %d %d %d\n", created, CGI.remote_user, CGI.remote_addr, CGI.remote_host, method, link.category, link.subject, link.id ) ;
 fclose ( logfile ) ;


  /* visa ett inlägg */
              
if ( method ==  SELECT )
  {
  
   selectLink ( link.category, link.subject, link.id, &link ) ;

   /* länka vidare */

   printLinkHTML ( link ) ;
   
}

if ( method ==  EDIT )
  {
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny för konferanssystem */
   if ( link.id != 0 )    /* befintligt inlägg skall ändras  */
    {
     selectLink ( link.category, link.subject, link.id, &link ) ;   
    }
 
   if ( user.id > 0 )   /* om inte inloggad användare nollställ namn */
   {
      strcpy ( link.author, user.name ) ;
      strcpy ( link.email, user.epost ) ;
      link.owner = user.id ;
     }
 
  if ( strcmp ( EDITION, "Limited"))
    {
      if ( link.category > 0  )
        printLinkForm ( link ) ; /* skriv ut ett formulär för ett nytt meddelande */
     else
	 { 
	  category.id = link.category  ;
       printLinkCategoryForm ( category ) ;    /* välj vilken katagori som avses */
	 }
     }
  else
     printLinkForm ( link ) ; /* skriv ut ett formulär för ett nytt meddelande */

    catHTML ( FOOTER ) ;           /* skriv ut en sidfot i HTML        */
  }

  /* lägg till ett nytt inlägg */
                       
if ( method == INSERT )
  {
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */

   /* skriv ut meny för konferanssystem */
 
  if (!FRAMES ) 
   exit(0) ;
  
  if ( user.id == -1 )          /* det går inte att ändra ett inlägg om man inte är inloggad */ 
     exit (0) ;              
  
  link.owner = user.id ;
  insertLink ( &link ) ;     /* spara en ny länk */

  catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
 
  printLinkHTML ( link ) ;
 
  }

  /* visa ett inlägg */
                       
if ( method == DELETE )
  {
   catHTML ( HEADER ) ;          /* skriv ut ett sidhuvud i HTML     */
     if ( !FRAMES )
     exit(0) ;                   /* skriv ut meny för konferanssystem */

   if ( user.id == -1 )          /* det går inte att ändra ett inlägg om man inte är inloggad */ 
     exit (0) ;                

   deleteLink ( &link ) ;        /* radera länk  */
   catHTML ( HEADER ) ;          /* skriv ut ett sidhuvud i HTML     */
   exit ( FALSE ) ;
  }

if ( method == UPDATE )
  {
 
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
  
   if ( user.id == -1 )          /* det går inte att ändra ett inlägg om man inte är inloggad */ 
     exit (0) ;              

   updateLink( &link ) ;     /* spara länk */
     
   printLinkHTML ( link ) ; /* skriv ut artikel som HTML-kod    */
  
   /*   groupMail ( &link ) ;   */

   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
   exit ( TRUE ) ;
  
  }


return ( TRUE ) ;                     

}  /* end of main () */

int ExpandLink ( struct _CGIarg *CGIarg, struct _link *link ) 

  /* 
      Funktionen översätter argument från CGI-script till en post i en länkad lista
      
      Om inga fel inträffar returneras TRUE
      Om värde överskrider min eller maxvärde så returnerar funktionen FALSE
      Om någon annan typ av fel inträffar så returneras ERROR
      
      CGIarg innhåller en länkad lista med variabelnamn och värden
      
      *article innehåller en pekare till en post med värden. 
      Om värden saknas sätts NULL     

      Följande argument från formulär används av funktionen
       
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

  int size ;                           /* antal tecken som varialbel innehåller */
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
                       


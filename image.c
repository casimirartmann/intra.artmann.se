
/********************************************************

 Konferanssystem

 Program:%M%
 Version:  %I%
 Date:       %D%

 Programmet hanterar meddelanden  i ett konferanssystem

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
     expandArticle
  
     saveArticle               - spara artikel
     readArticle               - hämta en artikel
     readSubject               - hämta en eller flera rubriker 
     readNewsgroup             - hämta en eller fler namn på nyhetsgrupper

     printConferanceMenuHTML   - skriv ut tillgängliga menyval
     printNewsgroupHTML        - skriv ut valda nyhetsgrupper
     printSubjectHTML          - skriv ut valda rubriker
     printArticleHTML          - skriv ut en artikel
     printArticleForm          - skriv ut ett HTML-formulär för skriva en artikel         
  */
  

 /* huvudprogram message */

main ( int argc, char *argv[] )


{
char path[255] ;
int method ;                   /* vad skall programmet utföra */
char *instring ;               /* inläst teckensträng */

struct _CGI CGI ; 
struct _CGIarg CGIarg ; 

struct _user user ;            /* information om användaren */
struct _message message ;      /* aktuell artikel/artiklar */
struct _category category ;  /* kategori för inlägget */

  /* läs in CGI environment */

message.indent = 0 ;
message.comment = 0 ;
message.category = 0 ;
message.subject = 0 ;
message.id = 0 ;
message.owner = -1 ;
message.title[0] = (char) NULL ;
message.author[0] = (char) NULL ;
message.email[0] = (char) NULL ;
message.text[0] = (char) NULL ;
message.company[0] = (char) NULL ;
message.created[0] = (char) NULL ;
message.reply[0] = (char) NULL ;

strcpy ( path, argv[0] ) ;
InitIntra ( path ) ;

InitCGI () ;
GetCGI ( &instring, &CGI ) ;   

  /* konvertera värden från formulär */

bURLencode ( instring, &CGIarg ) ;
ExpandMethod ( &CGIarg, &method ) ;
ExpandMessage ( &CGIarg, &message ) ;

/* hämta information om användaren
    baserat på inloggning  via WWW-server */

   findUser ( CGI.remote_user, &user ) ;

  /* visa ett inlägg */
                       
if ( method == SELECT )
  {                     
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny för konferanssystem */
   selectMessage( message.category, message.subject, message.id, &message ) ;   
   
   if  (!FRAMES)
      exit(0) ;

   printMessageHTML ( user, message ) ; /* skriv ut artikel som HTML-kod    */


   catHTML ( FOOTER ) ;           /* skriv ut en sidfot i HTML        */
  }

  /* visa ett inlägg */
                       
if ( method ==  EDIT )
  {
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny för konferanssystem */

   if ( !FRAMES )
    exit(0) ;

   if ( message.id != 0 )    /* befintligt inlägg skall ändras  */    
    {
     selectMessage ( message.category, message.subject, message.id, &message ) ;   
    }

 
   if ( user.id > 0 )   /* om inte inloggad användare nollställ namn */
     {
      strcpy ( message.author, user.name ) ;
      strcpy ( message.email, user.epost ) ;
      message.owner = user.id ;
     }

   if ( strcmp ( EDITION, "Limited"))
   {
     if ( message.category > 0  )
       printMessageForm ( message ) ; /* skriv ut ett formulär för ett nytt meddelande */
	 else
	 { category.id = message.category  ; 
 	   printMessageCategoryForm ( category ) ;    /* välj vilken katagori som avses */
	 }
   }
   else
	 printMessageForm ( message ) ;

   catHTML ( FOOTER ) ;                         /* skriv ut en sidfot i HTML        */
  }

  /* lägg till ett nytt inlägg */
                       
if ( method == INSERT )
  {
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny för konferanssystem */
  if (!FRAMES ) 
   exit (0) ;

  message.owner = user.id ;       /* inlägg */
  insertMessage ( &message ) ;      /* spara artikel */
     
  printMessageHTML ( user, message ) ; /* skriv ut artikel som HTML-kod    */

  
  catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
  
  }

  /* visa ett inlägg */
                       
if ( method == DELETE )
  {
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
   if ( !FRAMES )
    exit(0) ;                                /* skriv ut meny för konferanssystem */
  
   deleteMessage ( &message ) ;      /* radera artikel */
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
   exit ( FALSE ) ;
  }

if ( method == UPDATE )
  {
  catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */

  if ( !FRAMES )
    exit(0) ; 

  updateMessage( &message ) ;     /* spara artikel */
     
  printMessageHTML ( user, message ) ; /* skriv ut artikel som HTML-kod    */
  
  
  catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
  
  }


return ( TRUE ) ;                     


}  /* end of main () */

int ExpandMessage ( struct _CGIarg *CGIarg, struct _message *message ) 

  /* 
      Funktionen översätter argument från CGI-script till en post i en länkad lista
      
      Om inga fel inträffar returneras TRUE
      Om värde överskrider min eller maxvärde så returnerar funktionen FALSE
      Om någon annan typ av fel inträffar så returneras ERROR
      
      CGIarg innhåller en länkad lista med variabelnamn och värden
      
      *message innehåller en pekare till en post med värden. 
      Om värden saknas sätts NULL     

      Följande argument från formulär används av funktionen
       
      id=        
      group=
      subject=
      title=
      public=
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
    message->category = atoi ( CGIarg->value ) ;
   }
  if ( !strncmp("id", CGIarg->name, 2 )) {
    message->id = atoi ( CGIarg->value ) ;
   }

  if ( !strncmp("group", CGIarg->name, 5 )) {
    message->group = atoi ( CGIarg->value ) ;
   }

  if ( !strncmp("subject", CGIarg->name, 7 )) {
    message->subject = atoi ( CGIarg->value ) ;
   }

   if ( !strncmp("target", CGIarg->name, 6 )) {
    message->target = atoi ( CGIarg->value ) ;
   }

  if ( !strncmp("public", CGIarg->name, 6 )) {
    message->security = atoi ( CGIarg->value ) ;
   }

  if ( !strncmp("comment", CGIarg->name, 7 )) {
    message->comment = atoi ( CGIarg->value ) ;
   }
  if ( !strncmp("indent", CGIarg->name, 6 )) {
    message->indent = atoi ( CGIarg->value ) ;
   }
  if ( !strncmp("title", CGIarg->name, 5 )) {
    strncpy( message->title, CGIarg->value, 64 ) ;
    message->title[size] = (char) NULL ;
   }

   if ( !strncmp("author", CGIarg->name, 6 )) {
    strncpy( message->author, CGIarg->value,64 ) ;
   message->author[size] = (char) NULL ;
   }
   if ( !strncmp("email", CGIarg->name, 4 )) {
    strncpy( message->email, CGIarg->value,128 ) ;
   message->email[size] = (char) NULL ;
   }
   if ( !strncmp("to", CGIarg->name, 2 )) {
    strncpy( message->to, CGIarg->value,128 ) ;
   message->to[size] = (char) NULL ;
   }
   if ( !strncmp("company", CGIarg->name, 7 )) {
    strncpy( message->company, CGIarg->value, 64 ) ;
   message->company[size] = (char) NULL ;
   }
   if ( !strncmp("text", CGIarg->name, 4 )) {
    strncpy( message->text, CGIarg->value,strlen(CGIarg->value) ) ;
   message->text[size] = (char) NULL ;
   }
  CGIarg = CGIarg->next ;
 }

CGIarg = first ;
/*free ( first ) ;*/

return ( TRUE ) ;

}   /* end of ExpandMessage */
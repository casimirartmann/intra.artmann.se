
/********************************************************

 Konferanssystem

 Program: message.c
 Version:  2.1.2.5
 Date:       07/02/06

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
     expandMessage
  
     insertMessage               - spara artikel
     selectMessage               - hämta en artikel
     readSubject               - hämta en eller flera rubriker 
     readNewsgroup             - hämta en eller fler namn på nyhetsgrupper

     printConferanceMenuHTML   - skriv ut tillgängliga menyval
     printNewsgroupHTML        - skriv ut valda nyhetsgrupper
     printSubjectHTML          - skriv ut valda rubriker
     printMessageHTML          - skriv ut en artikel
     printMessageForm          - skriv ut ett HTML-formulär för skriva en artikel         
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
struct _message message ;      /* aktuell meddelande */
struct _category category ;  /* kategori för inlägget */
struct _subject subject ;
struct _title title ;  

char created [26] ;
char filename[128] ; 

time_t today ;
FILE *logfile ;

  /* läs in CGI environment */

message.indent = 0 ;
message.comment = 0 ;
message.category = 0 ;
message.subject = 0 ;
message.id = 0 ;
message.owner = -1 ;
message.security = -1 ;
message.group = - 1 ;       /* user group */
message.title[0] = (char) NULL ;
message.author[0] = (char) NULL ;
message.email[0] = (char) NULL ;
message.text[0] = (char) NULL ;
message.company[0] = (char) NULL ;
message.created[0] = (char) NULL ;
message.reply[0] = (char) NULL ;
message.target = 0 ;

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

if (( logfile = fopen ( "/usr/local/apache/db/access.log", "a" )) == NULL )
   exit ( -1 ) ;
	
 time( &today ) ;
 strftime( created, 26,"%Y-%m-%d %H:%M:%S", localtime(&today) ) ;
 fprintf (logfile, "%s %s %s %s %d %d %d %d\n", created, CGI.remote_user, CGI.remote_addr, CGI.remote_host, method, message.category, message.subject, message.id ) ;
 fclose ( logfile ) ;

  /* visa ett inlägg */
                       
if ( method == SELECT )
  {                     
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny för konferanssystem */
    if ( !FRAMES )
     exit(0) ;

   selectMessage( message.category, message.subject, message.id, &message ) ;   
   
   	   /* om behšrighet == -1 sŒ skall den sŠttas till samma som ŠmnesomrŒdet */
    if ( message.security == -1 ) 
      message.security = getSubjectClass ( message.category, message.subject ) ;      
	   
    if ( validUser (user.id, message.security, method, message.owner, message.group ) != TRUE )
      printf("<cite>Select message for user %s is not allowed due to combination of Information Class, Owner and Group</cite><br>\n", user.name ) ;
    else
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
	   /* om behšrighet == -1 sŒ skall den sŠttas till samma som ŠmnesomrŒdet */

     if ( message.security == -1 ) 
       message.security = getSubjectClass ( message.category, message.subject ) ;      

	 if ( validUser (user.id, message.security, method, message.owner, message.group ) != TRUE )
        {
         printf("<cite>Edit message for user %s is not allowed due to combination of Information Class, Owner and Group</cite><br>\n", user.name ) ;
         printf("Errorinformation: %d %d %d %d %d<br>\n", user.id, message.security, method, message.owner, message.group ) ;
         exit(0) ;
        }   
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

    /* om satt behšrighetsklass Šr lŠgre Šn ŠmnesomrŒdet sŒ sŠtts behšrighetsklass till samma som ŠmnesomrŒdet */
	   
    if ( message.security < getSubjectClass ( message.category, message.subject)) 
	  message.security = getSubjectClass ( message.category, message.subject ) ;

    if ( validUser (user.id, message.security, method, message.owner, message.group ) != TRUE )
     {
      printf("<cite>Insert message for user %s is not allowed due to combination of Information Class, Owner and Group</cite><br>\n", user.name ) ;
      printf("Errorinformation: %d %d %d %d %d<br>\n", user.id, message.security, method, message.owner, message.group ) ;
      exit(0) ;
     }
 
  message.owner = user.id ;       /* inlägg */
  insertMessage ( &message ) ;      /* spara artikel */
     
  printMessageHTML ( user, message ) ; /* skriv ut artikel som HTML-kod    */


  /*groupMail ( &message ) ;*/

  catHTML ( FOOTER ) ;           /* skriv ut ett sidhuvud i HTML     */
  
  }

  /* visa ett inlägg */
                       
if ( method == DELETE )
  {
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                          /* skriv ut meny för konferanssystem */

   if ( !FRAMES )
     exit(0) ;
   
   if ( user.id == -1 )          /* det går inte att ändra ett inlägg om man inte är inloggad */ 
     exit (0) ;              
	 
    selectMessage ( message.category, message.subject, message.id, &message ) ;

    if ( validUser (user.id, message.security, method, message.owner, message.group ) != TRUE )
     {
      printf("<cite>Delete message for user %s is not allowed due to combination of Information Class, Owner and Group</cite><br>\n", user.name ) ;
	  printf("Errorinformation: %d %d %d %d %d<br>\n", user.id, message.security, method, message.owner, message.group ) ;
		
      exit(0) ;
     }

   deleteMessage ( &message ) ;      /* radera artikel */
   catHTML ( HEADER ) ;           /*user.id == -1 skriv ut ett sidhuvud i HTML     */
   exit ( FALSE ) ;

  }

if ( method == UPDATE )
  {
  catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */

  if ( user.id == -1 )          /* det går inte att ändra ett inlägg om man inte är inloggad */ 
     exit (0) ;              

  if ( !getTitle ( message.category, message.subject, message.id, &title ))
    {
     errorHTML ("Message not found\n") ;
     exit(0) ; 
     } 

    if ( validUser (user.id, title.security, method, title.owner, title.group ) != TRUE )
     {
      errorHTML("Update message for user is not allowed due to combination of Information Class, Owner and group\n" ) ;
	  exit(0) ;
     }
  
  message.owner = user.id ;
  message.group = title.group ;

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

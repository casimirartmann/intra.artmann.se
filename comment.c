/********************************************************

 Konferanssystem version 2.1
 
 Programmet används för att kommentera ett annat inlägg

   
 Program: comment.c
 Version:  2.1.1.7
 Date:      99/05/21 

 

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

  /* funktioner för hantering av cgi-anrop och felhantering */
  
#include "../cgi.h"
#include "../utils.h"

#include "intra.h"
#include "db.h"
#include "html.h"

  /* gemensamma funktioner för konferanssystemet */


/* följande funktioner anropas */ 


/* huvudprogram comment */

main ( int argc, char *argv[])
 {

char path[255] ;
int method ;
char *instring ; 
struct _CGI CGI ;
struct _CGIarg CGIarg ; 

struct _user user ;            /* information om användaren */
struct _message message ;
struct _title title ;
struct _category category ;

message.indent = 0 ;
message.category = 0 ;
message.subject = 0 ;
message.id = 0 ;
message.owner = -1 ;
message.title[0] = (char) NULL ;
message.author[0] = (char) NULL ;
message.email[0] = (char) NULL ;
message.text[0] = (char) NULL ;
message.created[0] = (char) NULL ;
message.company[0] = (char) NULL ;
message.comment = 0 ;
message.reply[0] = (char) NULL ;

strcpy ( path, argv[0] ) ;
InitIntra ( path ) ;

InitCGI () ;
GetCGI ( &instring, &CGI ) ;

bURLencode ( instring, &CGIarg ) ;

ExpandMethod ( &CGIarg, &method ) ;
ExpandComment ( &CGIarg, &message ) ;

findUser ( CGI.remote_user, &user ) ;

  /* släpp reserverat minne för formulärvärden */
  
 /* visa ett inlägg */
                       
if ( method == EDIT )
  {

	
	catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny för konferanssystem */
   if ( !FRAMES )
     exit(0) ;

   if ( message.id != 0 )    /* kommentar till tidigare inlägg */
    { 

      selectMessage ( message.category, message.subject, message.id, &message ) ;         

      message.comment = message.id ;
      message.indent ++ ;
      sprintf( title.name, "Re:%s", message.title ) ;
	  strcpy ( message.title, title.name ) ;

    } 

   if ( user.id > 0 )   /* om inte inloggad användare nollställ namn */
     {

      strcpy ( message.author, user.name ) ;
      strcpy ( message.email, user.epost ) ;
      message.owner = user.id ;
     }
   else
     {
      message.author[0] = (char) NULL ;
      message.email[0] = (char) NULL ;
      message.owner = -1 ;
     }
 
   if ( strcmp ( EDITION, "Limited"))
   {
     if ( message.category > 0  )
       printCommentForm ( message ) ; /* skriv ut ett formulär för ett nytt meddelande */
	 else
	 { category.id = message.category  ; 
 	   printCommentCategoryForm ( category ) ;    /* välj vilken katagori som avses */
	 }
   }
   else
	 printCommentForm ( message ) ;

   catHTML ( FOOTER ) ;           /* skriv ut en sidfot i HTML        */
  }

return ( TRUE ) ;                    

}  /* end of main () */ 

int ExpandComment ( struct _CGIarg *CGIarg, struct _message *message ) 

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

}   /* end of ExpandComment */

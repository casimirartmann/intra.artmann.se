
/********************************************************

 Konferanssystem version 2.1

 Programmet hanterar artiklar i ett konferanssystem

 Konferanssystem 

 program:  article.c
 version:  2.1.2.6
 date:     2007-01-24 
  
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
  
     insertArticle             - spara artikel
     updateArticle             -
	 selectArticle             - hämta en artikel
	 deleteArticle             -
     
     printArticleHTML          - skriv ut en artikel
     printArticleForm          - skriv ut ett HTML-formulär för skriva en artikel         

  */

 /* huvudprogram article */


main ( int argc, char *argv[] )


{
char path[255] ;
int method ;                   /* vad skall programmet utföra */
char *instring ;               /* inläst teckensträng */
 int counter ;                   /* antal inlägg som skall skrivas ut */

struct _CGI CGI ; 
struct _CGIarg CGIarg ; 

struct _user user ;                 /* information om användaren */
struct _article article ;           /* aktuell artikel/artiklar */

struct _category category ;  /* kategori för inlägget */
struct _subject cubject ;
struct _title title ;                 /* rubriker för nyhetsgruppen */

char created [26] ;
char filename[128] ; 

time_t today ;
FILE *logfile ;

  /* läs in CGI environment */

article.indent = 0 ;
article.category = 0 ;
article.subject = 0 ;
article.id = 0 ;
article.owner = -1 ;
article.group = -1 ;
article.security = -1 ;
article.title[0] = (char) NULL ;
article.author[0] = (char) NULL ;
article.email[0] = (char) NULL ;
article.text[0] = (char) NULL ;
article.company[0] = (char) NULL ;    
article.created[0] = (char) NULL ;
article.target = 0 ;


strcpy ( path, argv[0] ) ;
InitIntra ( path ) ;
InitCGI () ;
GetCGI ( &instring, &CGI ) ;   

  /* konvertera värden från formulär */

bURLencode ( instring, &CGIarg ) ;
ExpandMethod ( &CGIarg, &method ) ;
ExpandArticle ( &CGIarg, &article ) ;

/* hämta information om användaren
    baserat på inloggning  via WWW-server */

findUser ( CGI.remote_user, &user ) ;

if (( logfile = fopen ( "/usr/local/apache/db/access.log", "a" )) == NULL )
   exit ( -1 ) ;
	
 time( &today ) ;
 strftime( created, 26,"%Y-%m-%d %H:%M:%S", localtime(&today) ) ;
 fprintf (logfile, "%s %s %s article %d %d %d %d\n", created, CGI.remote_user, CGI.remote_addr, method, article.category, article.subject, article.id ) ;
 fclose ( logfile ) ;

  /* visa ett inlägg */
                       
if ( method == SELECT )
  {                     
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  
    if  (!FRAMES)                 /* skriv ut meny för konferanssystem */
      exit(0) ;
	
	   /* läs texten för en artikel */
	   
    selectArticle ( article.category, article.subject, article.id, &article ) ;
	
	   /* om behšrighet == -1 sŒ skall den sŠttas till samma som ŠmnesomrŒdet */
	if ( article.security == -1 ) 
      article.security = getSubjectClass ( article.category, article.subject ) ;      
	   
    if ( validUser (user.id, article.security, method, article.owner, article.group ) != TRUE )
	 {
	  printf("<cite>Select article for user %s is not allowed due to combination of Information Class, Owner and Group</cite><br>\n", user.name ) ;
	  printf("Error information: %d %d %d %d %d<br>\n", user.id, article.security, method, article.owner, article.group ) ;
      }  
	  else
      /* skriv ut artikel som HTML-kod    */
	  printArticleHTML ( user, article ) ; 
   
   catHTML ( FOOTER ) ;           /* skriv ut en sidfot i HTML        */
  }

  /* visa ett inlägg */
                       
if ( method ==  EDIT )
  {
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny för konferanssystem */
   if ( !FRAMES )
     exit(0) ;

     if ( article.id != 0 )        /* befintligt inlägg skall ändras  */
    {
	
     selectArticle ( article.category, article.subject, article.id, &article ) ;   

	   /* om behšrighet == -1 sŒ skall den sŠttas till samma som ŠmnesomrŒdet */
	 if ( article.security == -1 ) 
       article.security = getSubjectClass ( article.category, article.subject ) ;      

	 if ( validUser (user.id, article.security, method, article.owner, article.group ) != TRUE )
        {
         printf("<cite>Edit article for user %s is not allowed due to combination of Information Class, Owner and Group</cite><br>\n", user.name ) ;
		 printf("Error information: %d %d %d %d %d<br>\n", user.id, article.security, method, article.owner, article.group ) ;
         exit(0) ;
        }
      }
  
      /* ändra namn till inloggad användare  */
   
   if ( user.id > 0 )   /* om inte inloggad användare nollställ namn */
    {
     strcpy ( article.author, user.name ) ;
     strcpy ( article.email, user.epost ) ;
     article.owner = user.id ;
    }
 
  if ( strcmp ( EDITION, "Limited"))
    {
      if ( article.category > 0  )
        printArticleForm ( article ) ; /* skriv ut ett formulär för ett nytt meddelande */
     else
	 { 
	   category.id = article.category  ;
       printArticleCategoryForm ( category ) ;    /* välj vilken katagori som avses */
	 }
     }
  else
     printArticleForm ( article ) ; /* skriv ut ett formulär för ett nytt meddelande */

   catHTML ( FOOTER ) ;           /* skriv ut en sidfot i HTML        */
  }

  /* lägg till ett nytt inlägg */
                       
if ( method == INSERT )
 {
  catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny för konferanssystem */
    if (!FRAMES ) 
      exit(0) ;

       /* om satt behšrighetsklass Šr lŠgre Šn ŠmnesomrŒdet sŒ sŠtts behšrighetsklass till samma som ŠmnesomrŒdet */
	   
    if ( article.security < getSubjectClass ( article.category, article.subject)) 
	  article.security = getSubjectClass ( article.category, article.subject ) ;
	
    if ( validUser (user.id, article.security, method, article.owner, article.group ) != TRUE )
     {
      printf("<cite>Insert article for user %s is not allowed due to combination of Information Class, Owner and Group</cite><br>\n", user.name ) ;
      printf("Error information: %d %d %d %d %d<br>\n", user.id, article.security, method, article.owner, article.group ) ;
      exit(0) ;
     }
 
  article.owner = user.id ;    
  insertArticle ( &article ) ;     /* spara artikel */
  
  printArticleHTML ( user, article ) ; /* skriv ut artikel som HTML-kod    */
  
  /*groupMail ( &article ) ;*/
  
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
   
   selectArticle ( article.category, article.subject, article.id, &article ) ;   
 
   if ( validUser (user.id, article.security, method, article.owner, article.group ) != TRUE )
	{
	 printf("<cite>Delete article for user %s is not allowed due to combination of Information Class, Owner and Group</cite><br>\n", user.name ) ;
	 printf("Error information: %d %d %d %d %d<br>\n", user.id, article.security, method, article.owner, article.group ) ;

	 exit(0) ;
	 }

   deleteArticle ( &article ) ;      /* radera artikel */
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
   exit ( FALSE ) ;
  }

if ( method == UPDATE )
  {
  catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */

  if ( !FRAMES )
   exit ( 0 ) ;

  if ( user.id == -1 )          /* det går inte att ändra ett inlägg om man inte är inloggad */ 
     exit (0) ;         
	 
  if ( !getTitle ( article.category, article.subject, article.id, &title ))
   {
     errorHTML ("Article not found\n") ;
     exit(0) ; 
} 

    if ( validUser (user.id, title.security, method, title.owner, title.group ) != TRUE )
     {
      printf("<cite>Update article for user %s is not allowed due to combination of Information Class, Owner and group</cite><br>\n", user.name ) ;
	  printf("Error information: %d %d %d %d %d<br>\n", user.id, title.security, method, title.owner, title.group ) ;
      exit(0) ;
     }
  
  article.owner = user.id ;
  article.group = title.group ;
  
  updateArticle( &article ) ;     /* spara artikel */
     
  printArticleHTML ( user, article ) ; /* skriv ut artikel som HTML-kod    */
  
  /* groupMail ( &article ) ;   */
  catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
  exit ( TRUE ) ;
  
  }
          
return ( TRUE ) ;                     

}  /* end of main () */

int ExpandArticle ( struct _CGIarg *CGIarg, struct _article *article ) 

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
      public= 
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
    article->category = atoi ( CGIarg->value ) ;
   }
  if ( !strncmp("id", CGIarg->name, 2 )) {
    article->id = atoi ( CGIarg->value ) ;
   }
  if ( !strncmp("group", CGIarg->name, 5 )) {
    article->group = atoi ( CGIarg->value ) ;
   }

   if ( !strncmp("target", CGIarg->name, 6 )) {
    article->target = atoi ( CGIarg->value ) ;
   }

  if ( !strncmp("subject", CGIarg->name, 7 )) {
    article->subject = atoi ( CGIarg->value ) ;
   }

  if ( !strncmp("public", CGIarg->name, 6 )) {
    article->security = atoi ( CGIarg->value ) ;
   }
   
  if ( !strncmp("comment", CGIarg->name, 7 )) {
    article->comment = atoi ( CGIarg->value ) ;
   }
  if ( !strncmp("title", CGIarg->name, 5 )) {
    strncpy( article->title, CGIarg->value, 64 ) ;
    article->title[size] = (char) NULL ;
   }

   if ( !strncmp("author", CGIarg->name, 6 )) {
    strncpy( article->author, CGIarg->value,64 ) ;
   article->author[size] = (char) NULL ;
   }

   if ( !strncmp("email", CGIarg->name, 4 )) {
    strncpy( article->email, CGIarg->value,128 ) ;
   article->email[size] = (char) NULL ;
   }
   if ( !strncmp("to", CGIarg->name, 2 )) {
    strncpy( article->to, CGIarg->value,128 ) ;
   article->to[size] = (char) NULL ;
   }
   if ( !strncmp("company", CGIarg->name, 7 )) {
    strncpy( article->company, CGIarg->value, 64 ) ;
   article->company[size] = (char) NULL ;
   }
   if ( !strncmp("text", CGIarg->name, 4 )) {
    strncpy( article->text, CGIarg->value,strlen(CGIarg->value) ) ;
   article->text[size] = (char) NULL ;
   }
  CGIarg = CGIarg->next ;
 }

CGIarg = first ;
/*free ( first ) ;*/

return ( TRUE ) ;

}   /* end of ExpandArticle */

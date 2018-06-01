
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
  
  /* funktioner f�r hantering av cgi-anrop och felhantering */

#include "../cgi.h"  
#include "../utils.h"

  /* gemensamma funktioner f�r konferanssystemet */
  
#include "intra.h"
#include "db.h"  
#include "html.h"

  /* f�ljande funktioner anropas  */
     
  /* expandMethod
     expandArticle
  
     insertArticle             - spara artikel
     updateArticle             -
	 selectArticle             - h�mta en artikel
	 deleteArticle             -
     
     printArticleHTML          - skriv ut en artikel
     printArticleForm          - skriv ut ett HTML-formul�r f�r skriva en artikel         

  */

 /* huvudprogram article */


main ( int argc, char *argv[] )


{
char path[255] ;
int method ;                   /* vad skall programmet utf�ra */
char *instring ;               /* inl�st teckenstr�ng */
 int counter ;                   /* antal inl�gg som skall skrivas ut */

struct _CGI CGI ; 
struct _CGIarg CGIarg ; 

struct _user user ;                 /* information om anv�ndaren */
struct _article article ;           /* aktuell artikel/artiklar */

struct _category category ;  /* kategori f�r inl�gget */
struct _subject cubject ;
struct _title title ;                 /* rubriker f�r nyhetsgruppen */

char created [26] ;
char filename[128] ; 

time_t today ;
FILE *logfile ;

  /* l�s in CGI environment */

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

  /* konvertera v�rden fr�n formul�r */

bURLencode ( instring, &CGIarg ) ;
ExpandMethod ( &CGIarg, &method ) ;
ExpandArticle ( &CGIarg, &article ) ;

/* h�mta information om anv�ndaren
    baserat p� inloggning  via WWW-server */

findUser ( CGI.remote_user, &user ) ;

if (( logfile = fopen ( "/usr/local/apache/db/access.log", "a" )) == NULL )
   exit ( -1 ) ;
	
 time( &today ) ;
 strftime( created, 26,"%Y-%m-%d %H:%M:%S", localtime(&today) ) ;
 fprintf (logfile, "%s %s %s article %d %d %d %d\n", created, CGI.remote_user, CGI.remote_addr, method, article.category, article.subject, article.id ) ;
 fclose ( logfile ) ;

  /* visa ett inl�gg */
                       
if ( method == SELECT )
  {                     
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  
    if  (!FRAMES)                 /* skriv ut meny f�r konferanssystem */
      exit(0) ;
	
	   /* l�s texten f�r en artikel */
	   
    selectArticle ( article.category, article.subject, article.id, &article ) ;
	
	   /* om beh�righet == -1 s� skall den s�ttas till samma som �mnesomr�det */
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

  /* visa ett inl�gg */
                       
if ( method ==  EDIT )
  {
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny f�r konferanssystem */
   if ( !FRAMES )
     exit(0) ;

     if ( article.id != 0 )        /* befintligt inl�gg skall �ndras  */
    {
	
     selectArticle ( article.category, article.subject, article.id, &article ) ;   

	   /* om beh�righet == -1 s� skall den s�ttas till samma som �mnesomr�det */
	 if ( article.security == -1 ) 
       article.security = getSubjectClass ( article.category, article.subject ) ;      

	 if ( validUser (user.id, article.security, method, article.owner, article.group ) != TRUE )
        {
         printf("<cite>Edit article for user %s is not allowed due to combination of Information Class, Owner and Group</cite><br>\n", user.name ) ;
		 printf("Error information: %d %d %d %d %d<br>\n", user.id, article.security, method, article.owner, article.group ) ;
         exit(0) ;
        }
      }
  
      /* �ndra namn till inloggad anv�ndare  */
   
   if ( user.id > 0 )   /* om inte inloggad anv�ndare nollst�ll namn */
    {
     strcpy ( article.author, user.name ) ;
     strcpy ( article.email, user.epost ) ;
     article.owner = user.id ;
    }
 
  if ( strcmp ( EDITION, "Limited"))
    {
      if ( article.category > 0  )
        printArticleForm ( article ) ; /* skriv ut ett formul�r f�r ett nytt meddelande */
     else
	 { 
	   category.id = article.category  ;
       printArticleCategoryForm ( category ) ;    /* v�lj vilken katagori som avses */
	 }
     }
  else
     printArticleForm ( article ) ; /* skriv ut ett formul�r f�r ett nytt meddelande */

   catHTML ( FOOTER ) ;           /* skriv ut en sidfot i HTML        */
  }

  /* l�gg till ett nytt inl�gg */
                       
if ( method == INSERT )
 {
  catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny f�r konferanssystem */
    if (!FRAMES ) 
      exit(0) ;

       /* om satt beh�righetsklass �r l�gre �n �mnesomr�det s� s�tts beh�righetsklass till samma som �mnesomr�det */
	   
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

  /* visa ett inl�gg */
                       
if ( method == DELETE )
  {
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny f�r konferanssystem */
   if ( !FRAMES )
     exit(0) ;
   
   if ( user.id == -1 )          /* det g�r inte att �ndra ett inl�gg om man inte �r inloggad */ 
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

  if ( user.id == -1 )          /* det g�r inte att �ndra ett inl�gg om man inte �r inloggad */ 
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

  int size ;                           /* antal tecken som varialbel inneh�ller */
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

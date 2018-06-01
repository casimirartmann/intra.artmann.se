/********************************************************

 Konferanssystem version 2.1

 Programmet hanterar referenser till andra publikationer

 Konferanssystem 

 program:  %M%
 version:  %I%
 date:     %D%
  
 written by Casimir Artmann.
  
 Copyright Casimir Artmann. 1996-1998.
 
*********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define TRUE 1
#define FALSE 0 

#define BOOK 1
#define MAGAZINE 2
#define LP 4
#define AUDIO_TAPE 5
#define VIDEO_TAPE 6
#define DAT  7
#define CD 8
#define DVD 9

 
  /* funktioner f�r hantering av cgi-anrop och felhantering */

#include "../cgi.h"  
#include "../utils.h"

  /* gemensamma funktioner f�r konferanssystemet */
  
#include "intra.h"
#include "db.h"  
#include "html.h"

  /* f�ljande funktioner anropas  */
     
  /* expandMethod
     expandpublication
  
     insertpublication             - spara artikel
     updatepublication             -
	 selectpublication             - h�mta en artikel
	 deletepublication             -
     
     printPublicationHTML          - skriv ut en artikel
     printPublicationForm          - skriv ut ett HTML-formul�r f�r skriva en artikel         

  */

 /* huvudprogram publication */


main ( int argc, char *argv[] )


{
char path[255] ;
int method ;                   /* vad skall programmet utf�ra */
char *instring ;               /* inl�st teckenstr�ng */
 int counter ;                   /* antal inl�gg som skall skrivas ut */

struct _CGI CGI ; 
struct _CGIarg CGIarg ; 

struct _user user ;                 /* information om anv�ndaren */
struct _publication publication ;           /* aktuell artikel/artiklar */
 struct _category category ;  /* kategori f�r inl�gget */
struct _title *title ;                 /* rubriker f�r nyhetsgruppen */


  /* l�s in CGI environment */

publication.indent = 0 ;
publication.category = 0 ;
publication.subject = 0 ;
publication.id = 0 ;
publication.owner = -1 ;
publication.security = -1 ;
publication.group = -1 ;
publication.title[0] = (char) NULL ;
publication.author[0] = (char) NULL ;
publication.email[0] = (char) NULL ;
publication.text[0] = (char) NULL ;
publication.company[0] = (char) NULL ;    
publication.created[0] = (char) NULL ;
publication.comment = 0 ;

strcpy ( path, argv[0] ) ;
InitIntra ( path ) ;

InitCGI () ;
GetCGI ( &instring, &CGI ) ;   

  /* konvertera v�rden fr�n formul�r */

bURLencode ( instring, &CGIarg ) ;
ExpandMethod ( &CGIarg, &method ) ;
ExpandPublication ( &CGIarg, &publication ) ;

/* h�mta information om anv�ndaren
    baserat p� inloggning  via WWW-server */

findUser ( CGI.remote_user, &user ) ;
	   
  /* visa ett inl�gg */
                       
if ( method == SELECT )
  {                     
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny f�r konferanssystem */
	if  (!FRAMES)
      exit(0) ;
	              
     /* l�s texten f�r en artikel */
    
    selectPublication ( publication.category, publication.subject, publication.id, &publication ) ;   

   /* om beh�righet == -1 s� skall den s�ttas till samma som �mnesomr�det */
	if ( publication.security == -1 ) 
      publication.security = getSubjectClass ( publication.category, publication.subject ) ;      
	   
    if ( validUser (user.id, publication.security, method, publication.owner, publication.group ) != TRUE )
	 {
	  printf("<cite>Select publication for user %s is not allowed due to combination of Information Class, Owner and Group</cite><br>\n", user.name ) ;
	  printf("Error information: %d %d %d %d %d<br>\n", user.id, publication.security, method, publication.owner, publication.group ) ;
      }  
	  else
      /* skriv ut artikel som HTML-kod    */

    /* skriv ut artikel som HTML-kod    */
    printPublicationHTML ( user, publication ) ; 

   catHTML ( FOOTER ) ;           /* skriv ut en sidfot i HTML        */
  }

  /* visa ett inl�gg */
                       
if ( method ==  EDIT )
  {
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny f�r konferanssystem */

   if ( !FRAMES )
     exit(0) ;

   if ( publication.id != 0 )    /* befintligt inl�gg skall �ndras  */
    {
     selectPublication ( publication.category, publication.subject, publication.id, &publication ) ;   
 
	 if ( publication.security == -1 ) 
       publication.security = getSubjectClass ( publication.category, publication.subject ) ;      

	 if ( validUser (user.id, publication.security, method, publication.owner, publication.group ) != TRUE )
        {
         printf("<cite>Edit publication for user %s is not allowed due to combination of Information Class, Owner and Group</cite><br>\n", user.name ) ;
		 printf("Error information: %d %d %d %d %d<br>\n", user.id, publication.security, method, publication.owner, publication.group ) ;
         exit(0) ;
        }
   }
 
  publication.owner = user.id ;
 
  if ( strcmp ( EDITION, "Limited"))
    {
      if ( publication.category > 0  )
        printPublicationForm ( publication ) ; /* skriv ut ett formul�r f�r ett nytt meddelande */
     else
	 { 
	   category.id = publication.category  ;
       printPublicationCategoryForm ( category ) ;    /* v�lj vilken katagori som avses */
	 }
     }
  else
     printPublicationForm ( publication ) ; /* skriv ut ett formul�r f�r ett nytt meddelande */

   catHTML ( FOOTER ) ;           /* skriv ut en sidfot i HTML        */
  }

  /* l�gg till ett nytt inl�gg */
                       
if ( method == INSERT )
  {
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny f�r konferanssystem */
  if (!FRAMES ) 
   exit(0) ;

   if ( publication.security < getSubjectClass ( publication.category, publication.subject)) 
	  publication.security = getSubjectClass ( publication.category, publication.subject ) ;
	
    if ( validUser (user.id, publication.security, method, publication.owner, publication.group ) != TRUE )
     {
      printf("<cite>Insert publication for user %s is not allowed due to combination of Information Class, Owner and Group</cite><br>\n", user.name ) ;
      printf("Error information: %d %d %d %d %d<br>\n", user.id, publication.security, method, publication.owner, publication.group ) ;
      exit(0) ;
     }

  publication.owner = user.id ;    
  insertPublication ( &publication ) ;     /* spara artikel */
  
  printPublicationHTML ( user, publication ) ; /* skriv ut artikel som HTML-kod    */
  
  /*groupMail ( &publication ) ;*/
  
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

   if ( validUser (user.id, publication.security, method, publication.owner, publication.group ) != TRUE )
	{
	 printf("<cite>Delete publication for user %s is not allowed due to combination of Information Class, Owner and Group</cite><br>\n", user.name ) ;
	 printf("Error information: %d %d %d %d %d<br>\n", user.id, publication.security, method, publication.owner, publication.group ) ;

	 exit(0) ;
	 }

   deletePublication ( &publication ) ;      /* radera artikel */
   catHTML ( FOOTER ) ;           /* skriv ut ett sidhuvud i HTML     */
   exit ( FALSE ) ;
  }

if ( method == UPDATE )
  {
  catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
  if ( !FRAMES )
    exit ( 0 ) ;

  updatePublication( &publication ) ;     /* spara artikel */
     
  printPublicationHTML ( user, publication ) ; /* skriv ut artikel som HTML-kod    */
  
  /* groupMail ( &publication ) ; */
    
  catHTML ( FOOTER ) ;           /* skriv ut ett sidhuvud i HTML     */
  exit ( TRUE ) ;
  
  }

return ( TRUE ) ;                     

}  /* end of main () */



int ExpandPublication ( struct _CGIarg *CGIarg, struct _publication *publication ) 

  /* 
      Funktionen �vers�tter argument fr�n CGI-script till en post i en l�nkad lista
      
      Om inga fel intr�ffar returneras TRUE
      Om v�rde �verskrider min eller maxv�rde s� returnerar funktionen FALSE
      Om n�gon annan typ av fel intr�ffar s� returneras ERROR
      
      CGIarg innh�ller en l�nkad lista med variabelnamn och v�rden
      
      *publication inneh�ller en pekare till en post med v�rden. 
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
    publication->category = atoi ( CGIarg->value ) ;
   }
  if ( !strncmp("id", CGIarg->name, 2 )) {
    publication->id = atoi ( CGIarg->value ) ;
   }
  
  if ( !strncmp("subject", CGIarg->name, 7 )) {
    publication->subject = atoi ( CGIarg->value ) ;
   }

  if ( !strncmp("public", CGIarg->name, 6 )) {
    publication->security = atoi ( CGIarg->value ) ;
   }

 if ( !strncmp("group", CGIarg->name, 5 )) {
    publication->group = atoi ( CGIarg->value ) ; 
   }

  if ( !strncmp("media", CGIarg->name, 5 )) {
    publication->media = atoi ( CGIarg->value ) ;
   }

  if ( !strncmp("title", CGIarg->name, 5 )) {
    strcpy( publication->title, CGIarg->value ) ;
    publication->title[size] = (char) NULL ;
   }

   if ( !strncmp("author", CGIarg->name, 6 )) {
    strcpy( publication->author, CGIarg->value ) ;
   publication->author[size] = (char) NULL ;
   }
   if ( !strncmp("email", CGIarg->name, 4 )) {
    strcpy( publication->email, CGIarg->value ) ;
   publication->email[size] = (char) NULL ;
   }
   if ( !strncmp("to", CGIarg->name, 2 )) {
    strcpy( publication->to, CGIarg->value ) ;
   publication->to[size] = (char) NULL ;
   }
   if ( !strncmp("company", CGIarg->name, 7 )) {
    strcpy( publication->company, CGIarg->value ) ;
   publication->company[size] = (char) NULL ;
   }
   if ( !strncmp("text", CGIarg->name, 4 )) {
    strcpy( publication->text, CGIarg->value ) ;
   publication->text[size] = (char) NULL ;
   }
  CGIarg = CGIarg->next ;
 }

CGIarg = first ;
/*free ( first ) ;*/

return ( TRUE ) ;

}   /* end of ExpandPublication */


/********************************************************

 Intra 2.1

 Program: %M%
 Version: %I%
 Date:    %D%

 Programmet hanterar ljudfiler i ett konferanssystem

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
  
     saveAudio               - spara artikel
     readAudio               - hämta en artikel
     readSubject               - hämta en eller flera rubriker 
     readCategory             - hämta en eller fler namn på nyhetsgrupper


     */
  

 /* huvudprogram audio */

main ( int argc, char *argv[] )


{
char path[255] ;
int method ;                   /* vad skall programmet utföra */
char *instring ;               /* inläst teckensträng */

struct _CGI CGI ; 
struct _CGIarg CGIarg ; 

struct _user user ;            /* information om användaren */
struct _audio audio ;      /* aktuell artikel/artiklar */
struct _category category ;  /* kategori för inlägget */

  /* läs in CGI environment */

audio.indent = 0 ;
audio.comment = 0 ;
audio.category = 0 ;
audio.subject = 0 ;
audio.id = 0 ;
audio.owner = -1 ;
audio.title[0] = (char) NULL ;
audio.author[0] = (char) NULL ;
audio.email[0] = (char) NULL ;
audio.text[0] = (char) NULL ;
audio.company[0] = (char) NULL ;
audio.created[0] = (char) NULL ;
audio.reply[0] = (char) NULL ;

strcpy ( path, argv[0] ) ;
InitIntra ( path ) ;
InitCGI () ;
GetCGI ( &instring, &CGI ) ;    

  /* konvertera värden från formulär */

bURLencode ( instring, &CGIarg ) ;
ExpandMethod ( &CGIarg, &method ) ;
ExpandAudio ( &CGIarg, &audio ) ;

/* hämta information om användaren
    baserat på inloggning  via WWW-server */

   findUser ( CGI.remote_user, &user ) ;

  /* visa ett inlägg */
                       
if ( method == SELECT )
  {                     
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny för konferanssystem */
   selectAudio( audio.category, audio.subject, audio.id, &audio ) ;   
   
   if  (!FRAMES)
      exit(0) ;

   printAudioHTML ( user, audio ) ; /* skriv ut artikel som HTML-kod    */


   catHTML ( FOOTER ) ;           /* skriv ut en sidfot i HTML        */
  }

  /* visa ett inlägg */
                       
if ( method ==  EDIT )
  {
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny för konferanssystem */

   if ( !FRAMES )
    exit(0) ;

   if ( audio.id != 0 )    /* befintligt inlägg skall ändras  */    
    {
     selectAudio ( audio.category, audio.subject, audio.id, &audio ) ;   
    }

 
   if ( user.id > 0 )   /* om inte inloggad användare nollställ namn */
     {
      strcpy ( audio.author, user.name ) ;
      strcpy ( audio.email, user.epost ) ;
      audio.owner = user.id ;
     }

   if ( strcmp ( EDITION, "Limited"))
   {
     if ( audio.category > 0  )
       printAudioForm ( audio ) ; /* skriv ut ett formulär för ett nytt meddelande */
	 else
	 { category.id = audio.category  ; 
 	   printAudioCategoryForm ( category ) ;    /* välj vilken katagori som avses */
	 }
   }
   else
	 printAudioForm ( audio ) ;

   catHTML ( FOOTER ) ;                         /* skriv ut en sidfot i HTML        */
  }

  /* lägg till ett nytt inlägg */
                       
if ( method == INSERT )
  {
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny för konferanssystem */
  if (!FRAMES ) 
   exit (0) ;

  audio.owner = user.id ;       /* inlägg */
  saveAudio ( &audio ) ;      /* spara artikel */
     
  printAudioHTML ( user, audio ) ; /* skriv ut artikel som HTML-kod    */

  
  catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
  
  }

  /* visa ett inlägg */
                       
if ( method == DELETE )
  {
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
   if ( !FRAMES )
    exit(0) ;                                /* skriv ut meny för konferanssystem */
  
   deleteAudio ( &audio ) ;      /* radera artikel */
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
   exit ( FALSE ) ;
  }

if ( method == UPDATE )
  {
  catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */

  if ( !FRAMES )
    exit(0) ; 

  updateAudio( &audio ) ;     /* spara artikel */
     
  printAudioHTML ( user, audio ) ; /* skriv ut artikel som HTML-kod    */
  
  
  catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
  
  }


return ( TRUE ) ;                     


}  /* end of main () */

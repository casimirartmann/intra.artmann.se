/********************************************************

 Intranet  version 2.0

 Programmet hanterare information om anv�ndare

 Program:  profile.c
 Version:  2.1.2.4
 Date:     01/03/10

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
     

 /* huvudprogram about */

void printUserProfileHTML ( struct _user user ) ;

main ( int argc, char *argv[])


{

char path[255] ;
int method ;                   /* vad skall programmet utf�ra */
char *instring ;               /* inl�st teckenstr�ng */

struct _CGI CGI ; 
struct _CGIarg CGIarg ; 

struct _user user ;            /* information om anv�ndaren */

  /* l�s in CGI environment */

strcpy ( path, argv[0] ) ;
InitIntra ( path) ;
InitCGI () ;
GetCGI ( &instring, &CGI ) ;      

bURLencode ( instring, &CGIarg ) ;
ExpandMethod ( &CGIarg, &method ) ;
  /* konvertera v�rden fr�n formul�r */

findUser ( CGI.remote_user, &user ) ;


if ( method == SELECT ) 
{
  catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                               /* skriv ut meny f�r konferanssystem */

  printUserProfileHTML ( user ) ;
  
  catHTML ( FOOTER ) ;           /* skriv ut ett sidhuvud i HTML     */
} 


if ( method == EDIT )
{
  catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                /* skriv ut meny f�r konferanssystem */
  if ( user.id == 0 ) 
    errorHTML ("Method not implemented") ;
  
  catHTML ( FOOTER ) ;           /* skriv ut ett sidhuvud i HTML     */
}

if ( method == INSERT )
{
  catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                               /* skriv ut meny f�r konferanssystem */

  if ( user.id == 0 ) 
    errorHTML ("Method not implemented") ;  
  
  catHTML ( FOOTER ) ;           /* skriv ut ett sidhuvud i HTML     */
}

if ( method == UPDATE )
{
  catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                               /* skriv ut meny f�r konferanssystem */

   if ( user.id == 0 ) 
    errorHTML ("Method not implemented") ;
  
  catHTML ( FOOTER ) ;           /* skriv ut ett sidhuvud i HTML     */
}

return ( TRUE ) ;                     


}  /* end of main () */


void printUserProfileHTML ( struct _user user ) 

{

printf("<H3>Anv�ndare</H3>\n") ;

printf ("Inloggningsnamn: %s, id: %d<br>\n", user.login, user.id ) ;
printf ("Anv�ndare: %s<br>", user.name ) ;
printf("F�retag: %s<br>", user.company ) ;
printf ("E-post:  %s<br>", user.epost ) ;
printf ("Hemsida: %s<br>", user.homepage ) ;

}

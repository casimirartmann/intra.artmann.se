/********************************************************

 Intranet  version 2.0

 Programmet hanterare information om användare

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
  
  /* funktioner för hantering av cgi-anrop och felhantering */

#include "../cgi.h"  
#include "../utils.h"

  /* gemensamma funktioner för konferanssystemet */
  
#include "intra.h"

#include "db.h"   
#include "html.h"

  /* följande funktioner anropas  */
     

 /* huvudprogram about */

void printUserProfileHTML ( struct _user user ) ;

main ( int argc, char *argv[])


{

char path[255] ;
int method ;                   /* vad skall programmet utföra */
char *instring ;               /* inläst teckensträng */

struct _CGI CGI ; 
struct _CGIarg CGIarg ; 

struct _user user ;            /* information om användaren */

  /* läs in CGI environment */

strcpy ( path, argv[0] ) ;
InitIntra ( path) ;
InitCGI () ;
GetCGI ( &instring, &CGI ) ;      

bURLencode ( instring, &CGIarg ) ;
ExpandMethod ( &CGIarg, &method ) ;
  /* konvertera värden från formulär */

findUser ( CGI.remote_user, &user ) ;


if ( method == SELECT ) 
{
  catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                               /* skriv ut meny för konferanssystem */

  printUserProfileHTML ( user ) ;
  
  catHTML ( FOOTER ) ;           /* skriv ut ett sidhuvud i HTML     */
} 


if ( method == EDIT )
{
  catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                /* skriv ut meny för konferanssystem */
  if ( user.id == 0 ) 
    errorHTML ("Method not implemented") ;
  
  catHTML ( FOOTER ) ;           /* skriv ut ett sidhuvud i HTML     */
}

if ( method == INSERT )
{
  catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                               /* skriv ut meny för konferanssystem */

  if ( user.id == 0 ) 
    errorHTML ("Method not implemented") ;  
  
  catHTML ( FOOTER ) ;           /* skriv ut ett sidhuvud i HTML     */
}

if ( method == UPDATE )
{
  catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                               /* skriv ut meny för konferanssystem */

   if ( user.id == 0 ) 
    errorHTML ("Method not implemented") ;
  
  catHTML ( FOOTER ) ;           /* skriv ut ett sidhuvud i HTML     */
}

return ( TRUE ) ;                     


}  /* end of main () */


void printUserProfileHTML ( struct _user user ) 

{

printf("<H3>Användare</H3>\n") ;

printf ("Inloggningsnamn: %s, id: %d<br>\n", user.login, user.id ) ;
printf ("Användare: %s<br>", user.name ) ;
printf("Företag: %s<br>", user.company ) ;
printf ("E-post:  %s<br>", user.epost ) ;
printf ("Hemsida: %s<br>", user.homepage ) ;

}

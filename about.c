/********************************************************

 Intranet  version 2.1.2

 Programmet visar information om Intranet

 Program: %M%
 Version: %I%
 Date:    %D%


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

void printAboutHTML ( struct _user user, struct _CGI CGI ) ;

main ( int argc, char *argv[] )


{

char path[255] ;
char *instring ;               /* inl�st teckenstr�ng */

struct _CGI CGI ; 

struct _user user ;            /* information om anv�ndaren */

  /* l�s in CGI environment */

strcpy ( path, argv[0] ) ;
InitIntra ( path ) ;

InitCGI () ;
GetCGI ( &instring, &CGI ) ;   

user.id = -1 ;
user.name[0] = (char) NULL ;
user.epost[0] = (char) NULL ;
user.company[0] = (char) NULL ;
user.homepage[0] = (char) NULL ;

  /* konvertera v�rden fr�n formul�r */

findUser ( CGI.remote_user, &user ) ;

  /* visa ett inl�gg */
                       
       
catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */

                                               /* skriv ut meny f�r konferanssystem */

printAboutHTML ( user, CGI ) ;
  
catHTML ( FOOTER ) ;           /* skriv ut ett sidhuvud i HTML     */

return ( TRUE ) ;                     


}  /* end of main () */


void printAboutHTML ( struct _user user, struct _CGI CGI ) 

{

printf("Version %s<br>\n", VERSION ) ;
printf("%s edition<br>\n", EDITION ) ;
printf ("%s<br>\n", LANGUAGE ) ;
printf ("<p>\n") ;

if ( user.id > 0 ) 
{
 printf ("User name: %s<br>\n", user.name ) ;
 printf ("Password: ******\n") ;
 printf ("Company %s<br>\n", user.company ) ;
 printf ("E-mail: %s<br>\n", user.epost ) ;
 printf ("Homepage: %s<br>\n", user.homepage ) ;
 
}
else
{ 
	printf ("Annonym anv�ndare<br>\n") ;
}

printf("<p>\n") ;
printf("Authentication: %s<br>\n", CGI.auth_type ) ;
printf("Browser: %s<br>\n", CGI.user_agent ) ;
printf("Remote user: %s<br>\n", CGI.remote_user ) ;
printf("Remote host: %s<br>\n", CGI.remote_host ) ;
printf("Remote identity: %s<br>\n", CGI.remote_ident ) ;
printf("Remote address: %s<br>\n", CGI.remote_addr ) ;
printf("<p>\n") ;
printf("Server name: %s<br>\n", CGI.server_name ) ;
printf("Server software: %s<br>\n", CGI.server_software ) ;
printf("Server software: %s<br>\n", CGI.server_protocol ) ;
printf("Server port: %s<br>\n", CGI.server_port ) ;



}  /* end of printAboutHTML */

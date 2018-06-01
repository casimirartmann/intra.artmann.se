/**********************************************************************/
/*                                                                    */
/*  test-script 1.0                                                   */
/*                                                                    */
/*                                                                    */
/*  Programmet används för att testa formulör, samt kod för 
    CGI-script                                                                   */
/*                                                                    */
/*                                                                    */
/*                                                                    */
/*                                                                    */
/*                                                                    */
/**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TRUE 1
#define FALSE 0
#define ERROR -1

#include "cgi.h"
#include "utils.h"

main ()


{

char *instring ;
int size ;

struct _CGI CGI ; 
struct _CGIarg CGIarg ;


  /* hŠmta vŠrden frŒn formulŠr */

InitCGI () ;
printf("<HTML><TITLE>Results from test-script</TITLE>\n") ;
printf("<BODY bgcolor=\"#FFFFFF\">\n") ;
printf("<H2>Results from test-script</H2>") ; 

GetCGI ( &instring, &CGI ) ;

printf("<UL>") ;

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



printf ("</UL>") ;
printf("<P>") ;

if ( instring != NULL )
  printf("%s<p><hr>", instring ) ;


printf("</BODY></HTML>") ;

}

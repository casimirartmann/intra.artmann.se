#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define ERROR -1

#include "cgi.h"
#include "cgi.c"

main ()


{

char *instring ;
int size ;

struct _CGI CGI ; 

  /* hämta värden från formulär */

GetCGI ( &instring, &CGI ) ;

printf("<HTML><TITLE>CGI-script query</TITLE>\n") ;
printf("<BODY bgcolor=\"#FFFFFF\">\n") ;
printf("<H2>Results from CGI-script query</H2>") ; 
printf("%s", instring ) ;

printf("</BODY></HTML>") ;

}   /* end of main query */

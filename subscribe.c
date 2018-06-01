
/********************************************************

 Konferanssystem

 Program: subscribe.c
 Version: 2.1.2.4
 Date:  99/05/21

 Programmet hanterar prenumerationer i ett konferanssystem

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
     expandsubscribe
  
  */
  

 /* huvudprogram subscribe */

struct _subscribe {
   int id ;
   int user ;
   char from[128] ;
   int  category ;
   int  subject ;
   int  inbox ;
   int  mail ;
} ;


int ExpandSubscribe ( struct _CGIarg *CGIarg, struct _subscribe *subscribe ) ;
void printSubscribeForm ( struct _subscribe subscribe ) ;
void printSubscribeCategoryForm ( struct _category category ) ;

int saveSubscribe ( struct _subscribe *subscribe ) ;     /* spara artikel */


main ( int argc, char *argv[] ) 


{
char path[255] ;
int method ;                   /* vad skall programmet utföra */
char *instring ;               /* inläst teckensträng */

struct _CGI CGI ; 
struct _CGIarg CGIarg ; 

struct _user user ;            /* information om användaren */
struct _subscribe subscribe ;  /* rubriker för nyhetsgruppen */
struct _category category ;    /* kategori för inlägget */

  /* läs in CGI environment */

strcpy ( path, argv[0] ) ;
InitIntra ( path ) ;

category.id = 0 ;
subscribe.category = 0 ;
subscribe.subject = 0 ;
subscribe.id = 0 ;
subscribe.mail = 0;
subscribe.from[0] = (char) NULL ;
subscribe.inbox = 0 ;


InitCGI () ;
GetCGI ( &instring, &CGI ) ;    

  /* konvertera värden från formulär */

bURLencode ( instring, &CGIarg ) ;
ExpandMethod ( &CGIarg, &method ) ;
ExpandSubscribe ( &CGIarg, &subscribe ) ;

/* hämta information om användaren
    baserat på inloggning  via WWW-server */

   findUser ( CGI.remote_user, &user ) ;

  /* visa ett inlägg */
                                              
if ( method ==  EDIT )
  {
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny för konferanssystem */

   if ( !FRAMES )
     exit(0) ;

 
   if ( user.id > 0 )   /* om inte inloggad användare nollställ namn */
     {
      strcpy ( subscribe.from, user.epost ) ;
      subscribe.user = user.id ;
     }
   else
     {
     
      subscribe.user = -1 ;
     }

   if ( subscribe.category > 0  )
     printSubscribeForm ( subscribe ) ; /* skriv ut ett formulär för ett nytt meddelande */
  else
     printSubscribeCategoryForm ( category ) ;    /* välj vilken katagori som avses */
   catHTML ( FOOTER ) ;           /* skriv ut en sidfot i HTML        */
  }

  /* lägg till ett nytt inlägg */
                       
if ( method == INSERT )
  {
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny för konferanssystem */
  if (!FRAMES ) 
    exit(0) ;

  subscribe.user = user.id ;
  saveSubscribe ( &subscribe ) ;     /* spara artikel */

  catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
  
  }

  /* visa ett inlägg */
                       
if ( method == DELETE )
  {
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny för konferanssystem */
   
   errorHTML ("Funktionen är inte implementerad i denna version") ;
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
   exit ( FALSE ) ;
  }

if ( method == UPDATE )
  {
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */

   errorHTML ("Funktionen är inte implementerad i denna version") ;
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
   exit ( FALSE ) ;
  }


return ( TRUE ) ;                     


}  /* end of main () */

/****************************************************************/

int ExpandSubscribe ( struct _CGIarg *CGIarg, struct _subscribe *subscribe ) 

  /* 
      Funktionen översätter argument från CGI-script till en post i en länkad lista
      
      Om inga fel inträffar returneras TRUE
      Om värde överskrider min eller maxvärde så returnerar funktionen FALSE
      Om någon annan typ av fel inträffar så returneras ERROR
      
      CGIarg innhåller en länkad lista med variabelnamn och värden
      
      *subscribe innehåller en pekare till en post med värden. 
      Om värden saknas sätts NULL     

      Följande argument från formulär används av funktionen
       
      id=        
      user=
	  to=
      category=
      subject=
      inbox=
      mail=
         
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
    subscribe->category = atoi ( CGIarg->value ) ;
   }
  if ( !strncmp("id", CGIarg->name, 2 )) {
    subscribe->id = atoi ( CGIarg->value ) ;
   }
  if ( !strncmp("group", CGIarg->name, 5 )) {
    subscribe->subject = atoi ( CGIarg->value ) ;
   }
  if ( !strncmp("inbox", CGIarg->name, 5 )) {
    subscribe->inbox = 1 ;
   }
  if ( !strncmp("mail", CGIarg->name, 5 )) {
    subscribe->mail = 1 ;
   }
   if ( !strncmp("from", CGIarg->name, 4 )) {
    strncpy( subscribe->from, CGIarg->value,128 ) ;
   subscribe->from[size] = (char) NULL ;
   }
   
   
  CGIarg = CGIarg->next ;
 }

CGIarg = first ;
/*free ( first ) ;*/

return ( TRUE ) ;

}   /* end of ExpandSubscribe */


void printSubscribeForm ( struct _subscribe subscribe )

{

char sCategory[32] ;
char sNewsgroup[32] ;

struct _subject subject ;

if ( strcmp ( EDITION, "Limited" ))
  getCategoryName ( subscribe.category, sCategory ) ;

if ( subscribe.subject > 0 )
  getSubjectName ( subscribe.category, subscribe.subject, sNewsgroup ) ;

if ( subscribe.id != 0 )
  printf("<H2>Ändra prenumeration</H2>\n") ;
else
  printf("<H2>Ny prenumeration</H2>\n") ;

printf("<FORM method=\"post\" action=\"%s/subscribe%s\">\n", CGI_PATH, EXECUTABLE ) ; 

if ( subscribe.id  != 0 )
   printf( "<input type=hidden name=method value=\"update\"> \n") ;
else
  printf("<input type=hidden name=method value=\"insert\"> \n") ;

if ( strcmp ( EDITION, "Limited"))
  printf( "<input type=hidden name=category  value=\"%d\"> \n", subscribe.category ) ;

printf("<table border=0>\n") ;

if ( strcmp ( EDITION, "Limited"))
 {
  printf("<tr>") ;
  printf("<td>Kategori</td>") ;
  printf("<td> %s", sCategory ) ;

  printf("</td>") ;
  printf("</tr>\n") ;
 }

printf("<tr>") ;
printf("<td>Ämne</td>") ;
printf("<td>") ;

subject.category = subscribe.category ;

if ( subscribe.subject == 0 )
 {
  selectSubject ( &subject,"ALL","DATE", NULL ) ; /* läs önskade nyhetsgrupper från fil */
  SubjectListBoxHTML ( &subject ) ; 
  }
else
  {
   printf("%s", sNewsgroup ) ;
   printf("<input type=hidden name=group value=\"%d\">", subscribe.subject ) ;
  } 

printf("</td></td>\n") ;

 /* om annonym användare så skall programmet fråga efter namn och e-postadress */

printf ("<input type=hidden name=inbox value=\"on\">") ;
printf ("<input type=hidden name=mail value=\"on\">" ) ;

printf("<tr><td valign=\"top\">E-post:<td>") ;
TextBoxHTML ( "from", subscribe.from, 25 ) ;
printf("</td></tr>") ;




printf("<tr><td></td><td>" ) ;
printf("<input type=submit value=\"Spara \">") ;
printf("<input type=reset value=\"Avbryt\">") ;
printf ("</td></tr></table>") ;
printf ("</FORM>" ) ; 
}
          

void printSubscribeCategoryForm ( struct _category category )

{

printf("<H2>Kategori för prenumeration</H2>\n") ;

printf("<FORM method=\"post\" action=\"%s/subscribe%s\">\n", CGI_PATH, EXECUTABLE ) ; 
printf("<input type=hidden name=method value=\"edit\"> \n") ;

printf("<table border=0>\n") ;
printf("<tr>") ;
printf("<td>Kategori</td>") ;
printf("<td>") ;

selectCategory ( &category,"ALL","DATE", NULL ) ; /* läs önskade kategorier  från fil */

CategoryListBoxHTML ( &category ) ; 

printf("</td></tr>\n") ;
printf("<tr><td></td><td>" ) ;
printf("<input type=submit value=\"Forts&auml;tt \">") ;
printf("<input type=reset value=\"Avbryt\">") ;
printf ("</td></tr></table>") ;
printf ("</FORM>" ) ;
 
}

/* sparar en prenumeration via inlåda eller e-post för en användare */

int saveSubscribe ( struct _subscribe *subscribe ) 

{

FILE *fp, *log, *lock ;

char filename[128] ;
char string[128] ;

sprintf ( filename, "%s/subscribe.lck" , DB_PATH ) ;

if (( lock = fopen ( filename, "w" )) == NULL )
   {
    errorHTML(" Prenumeration kan ej sparas. Fel vid postlåsning") ;
    return ( FALSE ) ;
   }

sprintf ( filename, "%s/subscribe.id", DB_PATH ) ;

    /* hämta nummer för nästa kategori */

if (( log = fopen ( filename, "r" )) == NULL )
   {
    errorHTML ("Prenumeration kan ej sparas. Fel vid läsning av id") ;
    return ( -1 ) ;
   }

if ( fgets ( string, 512, log ) != NULL )          /* läs första från fil som innehåller löpnummer */
 {
  subscribe->id = atoi ( string ) ;    /* senaste löpnummer */
  subscribe->id ++ ;                   /* öka tal med ett */ 
 }
 
fclose ( log ) ;

    

    /* öppna id filen och spara ned nummer */

if (( log = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("Kategori kan ej sparas. Fel vid läsning av id") ;
    return ( -1 ) ;
   }

fprintf ( log, "%d", subscribe->id ) ;
    
    /* stäng filen för id */
                     
fclose ( log ) ;


sprintf( filename, "%s/subscribe.dat", DB_PATH ) ;

if (( fp = fopen ( filename, "a" )) == NULL )
 {
  printf("Filen %s kan ej öppnas\n",  filename ) ;
  exit ( FALSE ) ;
 } 

fprintf ( fp, "%d\t%s\t%d\t%d\t%d\t%d\t%d\n",
		 subscribe->id, subscribe->from, subscribe->category, subscribe->subject,
		 subscribe->inbox,subscribe->mail,subscribe->user ) ;

fclose ( fp ) ;

return ( TRUE ) ;
} 


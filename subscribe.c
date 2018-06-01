
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
  
  /* funktioner f�r hantering av cgi-anrop och felhantering */

#include "../cgi.h"  
#include "../utils.h"

  /* gemensamma funktioner f�r konferanssystemet */
  
#include "intra.h"
#include "db.h"  
#include "html.h"

  /* f�ljande funktioner anropas  */
     
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
int method ;                   /* vad skall programmet utf�ra */
char *instring ;               /* inl�st teckenstr�ng */

struct _CGI CGI ; 
struct _CGIarg CGIarg ; 

struct _user user ;            /* information om anv�ndaren */
struct _subscribe subscribe ;  /* rubriker f�r nyhetsgruppen */
struct _category category ;    /* kategori f�r inl�gget */

  /* l�s in CGI environment */

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

  /* konvertera v�rden fr�n formul�r */

bURLencode ( instring, &CGIarg ) ;
ExpandMethod ( &CGIarg, &method ) ;
ExpandSubscribe ( &CGIarg, &subscribe ) ;

/* h�mta information om anv�ndaren
    baserat p� inloggning  via WWW-server */

   findUser ( CGI.remote_user, &user ) ;

  /* visa ett inl�gg */
                                              
if ( method ==  EDIT )
  {
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny f�r konferanssystem */

   if ( !FRAMES )
     exit(0) ;

 
   if ( user.id > 0 )   /* om inte inloggad anv�ndare nollst�ll namn */
     {
      strcpy ( subscribe.from, user.epost ) ;
      subscribe.user = user.id ;
     }
   else
     {
     
      subscribe.user = -1 ;
     }

   if ( subscribe.category > 0  )
     printSubscribeForm ( subscribe ) ; /* skriv ut ett formul�r f�r ett nytt meddelande */
  else
     printSubscribeCategoryForm ( category ) ;    /* v�lj vilken katagori som avses */
   catHTML ( FOOTER ) ;           /* skriv ut en sidfot i HTML        */
  }

  /* l�gg till ett nytt inl�gg */
                       
if ( method == INSERT )
  {
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny f�r konferanssystem */
  if (!FRAMES ) 
    exit(0) ;

  subscribe.user = user.id ;
  saveSubscribe ( &subscribe ) ;     /* spara artikel */

  catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
  
  }

  /* visa ett inl�gg */
                       
if ( method == DELETE )
  {
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny f�r konferanssystem */
   
   errorHTML ("Funktionen �r inte implementerad i denna version") ;
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
   exit ( FALSE ) ;
  }

if ( method == UPDATE )
  {
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */

   errorHTML ("Funktionen �r inte implementerad i denna version") ;
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
   exit ( FALSE ) ;
  }


return ( TRUE ) ;                     


}  /* end of main () */

/****************************************************************/

int ExpandSubscribe ( struct _CGIarg *CGIarg, struct _subscribe *subscribe ) 

  /* 
      Funktionen �vers�tter argument fr�n CGI-script till en post i en l�nkad lista
      
      Om inga fel intr�ffar returneras TRUE
      Om v�rde �verskrider min eller maxv�rde s� returnerar funktionen FALSE
      Om n�gon annan typ av fel intr�ffar s� returneras ERROR
      
      CGIarg innh�ller en l�nkad lista med variabelnamn och v�rden
      
      *subscribe inneh�ller en pekare till en post med v�rden. 
      Om v�rden saknas s�tts NULL     

      F�ljande argument fr�n formul�r anv�nds av funktionen
       
      id=        
      user=
	  to=
      category=
      subject=
      inbox=
      mail=
         
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
  printf("<H2>�ndra prenumeration</H2>\n") ;
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
printf("<td>�mne</td>") ;
printf("<td>") ;

subject.category = subscribe.category ;

if ( subscribe.subject == 0 )
 {
  selectSubject ( &subject,"ALL","DATE", NULL ) ; /* l�s �nskade nyhetsgrupper fr�n fil */
  SubjectListBoxHTML ( &subject ) ; 
  }
else
  {
   printf("%s", sNewsgroup ) ;
   printf("<input type=hidden name=group value=\"%d\">", subscribe.subject ) ;
  } 

printf("</td></td>\n") ;

 /* om annonym anv�ndare s� skall programmet fr�ga efter namn och e-postadress */

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

printf("<H2>Kategori f�r prenumeration</H2>\n") ;

printf("<FORM method=\"post\" action=\"%s/subscribe%s\">\n", CGI_PATH, EXECUTABLE ) ; 
printf("<input type=hidden name=method value=\"edit\"> \n") ;

printf("<table border=0>\n") ;
printf("<tr>") ;
printf("<td>Kategori</td>") ;
printf("<td>") ;

selectCategory ( &category,"ALL","DATE", NULL ) ; /* l�s �nskade kategorier  fr�n fil */

CategoryListBoxHTML ( &category ) ; 

printf("</td></tr>\n") ;
printf("<tr><td></td><td>" ) ;
printf("<input type=submit value=\"Forts&auml;tt \">") ;
printf("<input type=reset value=\"Avbryt\">") ;
printf ("</td></tr></table>") ;
printf ("</FORM>" ) ;
 
}

/* sparar en prenumeration via inl�da eller e-post f�r en anv�ndare */

int saveSubscribe ( struct _subscribe *subscribe ) 

{

FILE *fp, *log, *lock ;

char filename[128] ;
char string[128] ;

sprintf ( filename, "%s/subscribe.lck" , DB_PATH ) ;

if (( lock = fopen ( filename, "w" )) == NULL )
   {
    errorHTML(" Prenumeration kan ej sparas. Fel vid postl�sning") ;
    return ( FALSE ) ;
   }

sprintf ( filename, "%s/subscribe.id", DB_PATH ) ;

    /* h�mta nummer f�r n�sta kategori */

if (( log = fopen ( filename, "r" )) == NULL )
   {
    errorHTML ("Prenumeration kan ej sparas. Fel vid l�sning av id") ;
    return ( -1 ) ;
   }

if ( fgets ( string, 512, log ) != NULL )          /* l�s f�rsta fr�n fil som inneh�ller l�pnummer */
 {
  subscribe->id = atoi ( string ) ;    /* senaste l�pnummer */
  subscribe->id ++ ;                   /* �ka tal med ett */ 
 }
 
fclose ( log ) ;

    

    /* �ppna id filen och spara ned nummer */

if (( log = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("Kategori kan ej sparas. Fel vid l�sning av id") ;
    return ( -1 ) ;
   }

fprintf ( log, "%d", subscribe->id ) ;
    
    /* st�ng filen f�r id */
                     
fclose ( log ) ;


sprintf( filename, "%s/subscribe.dat", DB_PATH ) ;

if (( fp = fopen ( filename, "a" )) == NULL )
 {
  printf("Filen %s kan ej �ppnas\n",  filename ) ;
  exit ( FALSE ) ;
 } 

fprintf ( fp, "%d\t%s\t%d\t%d\t%d\t%d\t%d\n",
		 subscribe->id, subscribe->from, subscribe->category, subscribe->subject,
		 subscribe->inbox,subscribe->mail,subscribe->user ) ;

fclose ( fp ) ;

return ( TRUE ) ;
} 


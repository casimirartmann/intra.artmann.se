
/********************************************************

 Konferanssystem version 2.1

 Programmet hanterar dokument i ett konferanssystem

  Konferanssystem 

 document:  document.c
 version:   2.1.2.5
 date:      2006-08-28
  
 written by Casimir Artmann.
  
 Copyright Casimir Artmann. 1996-1999.
 
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
     expandDocument
     saveDocument               - spara dokument
     readDocument               - h�mta en dokument
     readSubject               - h�mta en eller flera rubriker 
     readNewsgroup             - h�mta en eller fler namn p� nyhetsgrupper

     printConferanceMenuHTML   - skriv ut tillg�ngliga menyval
     printNewsgroupHTML        - skriv ut valda nyhetsgrupper
     printSubjectHTML          - skriv ut valda rubriker
     printDocumentHTML          - skriv ut ett dokument
     printDocumentForm          - skriv ut ett HTML-formul�r f�r skriva om ett dokument         
  */



int ExpandDocument ( struct _CGIarg *CGIarg, struct _document *document ) ;
int insertDocument ( struct _document *document ) ;
int selectDocument ( int category_id, int subject_id, int object_id, struct _document *document ) ;
int deleteDocument ( struct _document *document ) ;
int updateDocument ( struct _document *document ) ;

void printDocumentHTML ( struct _user user, struct _document document ) ;
void printDocumentForm ( struct _document document ) ;
void printDocumentCategoryForm ( struct _category category ) ;
int  createDocumenthref ( struct _document *document ) ;


 /* huvudprogram document */


main ( int argc, char *argv[] )


{

char path[255] ;
int method ;                   /* vad skall programmet utf�ra */
char *instring ;               /* inl�st teckenstr�ng */

struct _CGI CGI ; 
struct _CGIarg CGIarg ; 

struct _user user ;            /* information om anv�ndaren */
struct _document document ;      /* aktuellt dokument */
struct _category category ;  /* kategori f�r inl�gget */

   /* initiera huvudprogram */

document.category = 0 ;
document.subject = 0 ;
document.id = 0 ;
document.subclass = 0 ;
document.security = -1 ;
document.title[0] = (char) NULL ;
document.author[0] = (char) NULL ;
document.verified[0] = (char) NULL ;
document.approved[0] = (char) NULL ;
document.distribute[0] = (char) NULL ;
document.email[0] = (char) NULL ;
document.to[0] = (char) NULL ;
document.company[0] = (char) NULL ;
document.reply[0] = (char) NULL ;
document.created[0] = (char) NULL ;
document.version[0] =(char) NULL ;
document.href[0] =(char) NULL ;
document.filename[0] =(char) NULL ;
document.text[0] = (char) NULL ; 
document.indent = 0 ;


strcpy ( path, argv[0] ) ;
InitIntra ( path ) ;

  /* l�s in CGI environment */

InitCGI () ;
GetCGI ( &instring, &CGI ) ;    

  /* konvertera v�rden fr�n formul�r */

bURLencode ( instring, &CGIarg ) ;
ExpandMethod ( &CGIarg, &method ) ;
ExpandDocument ( &CGIarg, &document ) ;

/* h�mta information om anv�ndaren
    baserat p� inloggning  via WWW-server */

 findUser ( CGI.remote_user, &user ) ;

  /* visa ett inl�gg */
                       
if ( method == SELECT )
  {                     
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny f�r konferanssystem */
  if  (!FRAMES)
   exit (0) ;
           
 /* l�s alla rubriker f�r aktuell kategori och nyhetsgrupp
    f�r att d�refter h�mta information om aktullt dokument */

   selectDocument ( document.category, document.subject, document.id, &document ) ;   
   printDocumentHTML ( user, document ) ; /* skriv ut artikel som HTML-kod    */

   catHTML ( FOOTER ) ;           /* skriv ut en sidfot i HTML        */
  }

  /* visa ett inl�gg */
                       
if ( method ==  EDIT )
  {
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
   category.id = document.category ;
                                  /* skriv ut meny f�r konferanssystem */

   if ( !FRAMES )
     exit(0) ;

   if ( document.id != 0 )    /* befintligt inl�gg skall �ndras  */
    {
      
   if ( user.id > 0 )   /* om inte inloggad anv�ndare nollst�ll namn */
     {
      strcpy ( document.author, user.name ) ;
      document.owner = user.id ;
     }
   else
     {
      document.author[0] = (char) NULL ;
      document.owner = -1 ;
     }

   }

  if ( strcmp ( EDITION, "Limited"))
    {
      if ( document.category > 0  )
        printDocumentForm ( document ) ; /* skriv ut ett formul�r f�r ett nytt meddelande */
     else
        printDocumentCategoryForm ( category ) ;    /* v�lj vilken katagori som avses */
     }
  else
     printDocumentForm ( document ) ; /* skriv ut ett formul�r f�r ett nytt meddelande */

   catHTML ( FOOTER ) ;           /* skriv ut en sidfot i HTML        */
  }

  /* l�gg till ett nytt inl�gg */
                       
if ( method == INSERT )
  {
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny f�r konferanssystem */
   if (!FRAMES ) 
     exit(0) ;
  
   document.owner = user.id ;       
  
    /* skapa l�nknamn baserat p� filnamn om l�nk inte angiven
 
  if ( strlen ( document.href ) == 0 )
    createDocumenthref( &document ) ;   
*/

  insertDocument ( &document ) ;     /* spara artikel */
  printDocumentHTML ( user, document ) ; /* skriv ut artikel som HTML-kod    */
  
  /* groupMail ( &article ) ; */
 
  catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
  
  }

  /* visa ett inl�gg */
                       
if ( method == DELETE )
  {
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
                                  /* skriv ut meny f�r konferanssystem */
    if ( !FRAMES )
      exit(0) ;
   deleteDocument ( &document ) ;      /* radera artikel */
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */   
  }

if ( method == UPDATE )
  {
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
   if ( !FRAMES )
      exit(0) ;

/*
  if ( strlen ( document.href ) == 0 )
    createDocumenthref( &document ) ;   
 */
 
  updateDocument ( &document ) ;     /* spara artikel */
     
  printDocumentHTML ( user, document ) ; /* skriv ut artikel som HTML-kod    */
  
  catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */
  exit ( FALSE ) ;
 }

return ( TRUE ) ;                     


}  /* end of main () */


int ExpandDocument ( struct _CGIarg *CGIarg, struct _document *document ) 

  /* 
      Funktionen �vers�tter argument fr�n CGI-script till en post i en l�nkad lista
      
      Om inga fel intr�ffar returneras TRUE
      Om v�rde �verskrider min eller maxv�rde s� returnerar funktionen FALSE
      Om n�gon annan typ av fel intr�ffar s� returneras ERROR
      
      CGIarg innh�ller en l�nkad lista med variabelnamn och v�rden
      
      *document inneh�ller en pekare till en post med v�rden. 
      Om v�rden saknas s�tts NULL     

      F�ljande argument fr�n formul�r anv�nds av funktionen
       
      id=        
      group=
      title=
      reply=
      subclass=
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
    document->category = atoi ( CGIarg->value ) ;
   }
  if ( !strncmp("id", CGIarg->name, 2 )) {
    document->id = atoi ( CGIarg->value ) ;
   }
  if ( !strncmp("group", CGIarg->name, 5 )) {
    document->group = atoi ( CGIarg->value ) ;
   }
  if ( !strncmp("subject", CGIarg->name, 7 )) {
    document->subject = atoi ( CGIarg->value ) ;
   }
  if ( !strncmp("public", CGIarg->name, 7 )) {
    document->security = atoi ( CGIarg->value ) ;
   }
  if ( !strncmp("subclass", CGIarg->name, 7 )) {
    document->subclass = atoi ( CGIarg->value ) ;
   }
  if ( !strncmp("indent", CGIarg->name, 6 )) {
    document->indent = atoi ( CGIarg->value ) ;
   }
  if ( !strncmp("title", CGIarg->name, 5 )) {
    strncpy( document->title, CGIarg->value, 64 ) ;
    document->title[size] = (char) NULL ;
   }

  if ( !strncmp("date", CGIarg->name, 4 )) {
    strncpy( document->created, CGIarg->value, 20 ) ;
    document->created[size] = (char) NULL ;
   }
  
 /*
*/ 
   if ( !strncmp("version", CGIarg->name, 7 )) {
      strncpy( document->version, CGIarg->value, 12 ) ;
    document->version[size] = (char) NULL ;
   }

  /**/
  
  if ( !strncmp("filename", CGIarg->name, 8 )) {
       strncpy( document->filename, CGIarg->value, 64 ) ;
       document->filename[size] = (char) NULL ;
     }
  if ( !strncmp("href", CGIarg->name, 4 )) {
       strncpy( document->href, CGIarg->value, 128 ) ;
       document->href[size] = (char) NULL ;
     }

  if ( !strncmp("infoclass", CGIarg->name, 9 )) {
    document->security = atoi ( CGIarg->value ) ;
   }
  

   /**/
  if ( !strncmp("approved", CGIarg->name, 8 )) {
       strncpy( document->approved, CGIarg->value,64 ) ;
       document->approved[size] = (char) NULL ;
  
  }
  
  if ( !strncmp("verified", CGIarg->name, 8 )) {
    strncpy( document->verified, CGIarg->value,64 ) ;
   document->verified[size] = (char) NULL ;
   }
  
  if ( !strncmp("published", CGIarg->name, 9 )) {
    strncpy( document->distribute, CGIarg->value,255 ) ;
   document->distribute[size] = (char) NULL ;
   }

  /**/
  
  if ( !strncmp("author", CGIarg->name, 6 )) {
    strncpy( document->author, CGIarg->value,64 ) ;
   document->author[size] = (char) NULL ;
   }
  
  if ( !strncmp("email", CGIarg->name, 4 )) {
    strncpy( document->email, CGIarg->value,128 ) ;
   document->email[size] = (char) NULL ;
   }
  
  if ( !strncmp("to", CGIarg->name, 2 )) {
    strncpy( document->to, CGIarg->value,128 ) ;
   document->to[size] = (char) NULL ;
   }
  
  if ( !strncmp("company", CGIarg->name, 7 )) {
    strncpy( document->company, CGIarg->value, 64 ) ;
   document->company[size] = (char) NULL ;
   }
  
  if ( !strncmp("text", CGIarg->name, 4 )) {
    strncpy( document->text, CGIarg->value,strlen(CGIarg->value) ) ;
   document->text[size] = (char) NULL ;
   }
  CGIarg = CGIarg->next ;
 }

CGIarg = first ;
/*free ( first ) ;*/

return ( TRUE ) ;

}   /* end of ExpandDocument */


void printDocumentHTML ( struct _user user, struct _document document )

{

printf ("<H3>%s</H3>\n",document.title ) ;

  /* skriv ut inl�gg om det inneh�ller text */

if ( strlen ( document.text) > 0 )
 {  
  printfHTML(  document.text ) ;
 }

printf("<br>") ;

  /* skriv ut l�nken som filnamn om s�dant finnas, annars skrivs l�nken ut */

printf("Dokument: ") ;
if ( strlen ( document.filename ) > 0 )
  printf("<a href=\"%s\">%s</a> ", document.href, document.filename ) ;  
else
  printf("<a href=\"%s\">%s</a> ", document.href, document.href ) ;  

printf ("Version: %s", document.version ) ;

 /* om dokument �r kontrollerat eller godk�nt skall detta skrivas ut.
    om dokument saknar dessa f�lt skall inga f�ltnamn skrivas ut */

if (( strlen (document.verified) > 0 ) || ( strlen(document.approved) > 0 ))
  printf("Kontrollerad: %s  Godk�nd: %s<br>\n", document.verified, document.approved ) ;

printf("<address>") ;

  /* skriv ut vem som har skickat inl�gget */

if ( strlen ( document.company ) > 0 )
  {
    if ( strlen ( document.email ) > 0 )
      printf("F&ouml;rfattare: <a href=\"mailto:%s\">%s</a> - <strong>%s</strong><br>",
        document.email, document.author, document.company ) ;
    else
      printf("F&ouml;rfattare: %s - <strong>%s</strong><br>",
        document.author, document.company ) ;
  }
else
  {
   if ( strlen ( document.email ) > 0 )
     printf("F&ouml;rfattare: <a href=\"mailto:%s\">%s</a><br>",
        document.email, document.author ) ;
   else
       printf("F&ouml;rfattare: %s<br>",  document.author ) ;
  }

printf("Datum: %s<br></address>\n", document.created ) ;
printf ("</td></tr></table>") ;
printf("<P>") ;

printf("<table><tr><td>") ;
printf("<FORM method=\"post\" action=\"%s/subclass%s\">", CGI_PATH, EXECUTABLE ) ; 
printf("<input object=hidden name=method value=\"edit\"> \n") ;
printf("<input object=hidden name=category value=\"%d\">\n", document.category ) ;
printf("<input object=hidden name=subject value=\"%d\">\n", document.subject ) ;
printf("<input object=hidden name=id value=\"%d\">\n", document.id ) ; 
printf("<input object=submit value=\"Kommentera \">") ;

printf ("</FORM>" ) ;
printf("</td><td>") ;

/*
  f�r att kunna v�lja redigera eller ta bort g�ller f�ljande:
  om inloggad anv�ndare och �gare av artikel eller administrat�r 
  skall man kunna redigera artikel 
*/

if (((  document.owner == user.id ) && ( user.id > 0 )) || ( user.id == 1 ))
 {
  printf("<FORM method=\"get\" action=\"%s/document%s\">", CGI_PATH, EXECUTABLE ) ; 
  printf("<input object=hidden name=method value=\"edit\"> \n") ;
  printf("<input object=hidden name=category value=\"%d\">\n", document.category ) ;
  printf("<input object=hidden name=subject value=\"%d\">\n", document.subject ) ;
  printf("<input object=hidden name=id value=\"%d\">\n", document.id ) ; 
  printf("<input object=submit value=\"Redigera \">") ;

  printf ("</FORM>" ) ;
  printf("</td><td>") ;
  printf("<FORM method=\"get\" action=\"%s/document%s\">", CGI_PATH, EXECUTABLE ) ; 
  printf("<input object=hidden name=method value=\"delete\"> \n") ;
  printf("<input object=hidden name=category value=\"%d\">\n", document.category ) ;
  printf("<input object=hidden name=subject value=\"%d\">\n", document.subject ) ;
  printf("<input object=hidden name=id value=\"%d\">\n", document.id ) ; 
  printf("<input object=submit value=\"Ta bort \">") ;

  printf ("</FORM>" ) ;
 }   /* end of if */
printf ("</td></tr></table>") ;

}  /* end of printDocumentHTML */

 /* skriv information om ett inl�gg till en textfil */
 
insertDocument ( struct _document *document )

  /*
    
    Skriver ett inl�gg till tv� olika filer

    news-?.dat som inneh�ller titel
    msg-?-?.dat som inneh�ller sj�lva inl�gget

    rad 1	
	l�nk -> dokumentnummer -> version -> f�rfattare -> kontrollerad -> godk�nd -> distrubueras

    rad 2
	beskrivande text

    filen news-?.id inneh�ller r�knare f�r n�sta lediga post
       
    En l�sfil news-?.lock anv�nds f�r att kontrollera s� att inga andra
    processer skriver till titelfilen, eller r�knarfilen
    
    funktionen returnerar TRUE om inl�ggets sparats ned
    vid filfel s� returneras ERROR
    om l�sningsfilen inte kan skrivas returneras FALSE 
  
    inl�ggen sparas in omv�nd ordning med senaste inl�gget f�rst 
  */

{



char filename[128] ;    /* filnamn f�r fil som inneh�ller ett inl�gg */

FILE *fp, *lock;

struct _title title ;


    /* s� l�nge denna filen �r �ppna kan inga andra inl�gg i gruppen g�ras */

if ((lock = lockSubject ( document->category, document->subject )) == NULL ) 
  return ( FALSE ) ;

    /* h�mta nummer f�r n�sta inl�gg */

document->id = GetNextTitleId ( document->category, document->subject ) ;
if ( document->id == ERROR ) 
  return ( FALSE ) ;

    /* spara ned rubrik f�r inl�gg */

title.id = document->id ;
title.category = document->category ;
title.subject = document->subject ;
title.owner = document->owner ;
title.security = document->security ;
title.indent = 0 ;     
title.object = 3 ;                     /* dokument */

strcpy ( title.created, document->created ) ;
strcpy ( title.name, document->title ) ;
strcpy ( title.email, document->email ) ;
strcpy ( title.author, document->author ) ;
strcpy ( title.company, document->company ) ;
printf("%s", title.name ) ;

insertTitle ( title ) ;

    /* spara ned id, rubrik, f�rfattare och created i fil */        
                 
    /* �ppna filen f�r skrivning av en artikel */

if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/message-%d-%d-%d.dat",DB_PATH,  document->category, document->subject, document->id  ) ;
else 
  sprintf( filename, "%s/message-%d-%d.dat", DB_PATH, document->subject, document->id  ) ; 

printf("%s<br>\n", filename ) ;
if (( fp = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej skrivas.") ;
    return ( -1 ) ;
   }

  /* skriv inl�gg till fil fil 
  rad 1	l�nk -> dokumentnummer -> version -> f�rfattare -> kontrollerad
-> godk�nd -> distrubueras
rad 2->	beskrivande text
   */


fprintf( fp, "%s\t%s\t%s\t%s\t%s\t%s\t%s\n", document->href, document->filename, 
						document->version, document->author, 
						document->verified, document->approved,
						document->distribute ) ;

fprintf ( fp, "%s\n", document->text ) ;

fclose ( fp ) ;

  /* st�ng l�sningsfilen f�r id */    

unlockSubject ( lock ) ;

return ( TRUE ) ;

} /* end of insertDocument () */

/**********************************************************************/


selectDocument ( int category_id, int subject_id, int object_id, struct _document *document )
 
 /* document.conferance, document.id, document.subject 
    anv�nds f�r att avg�ra vilken fil som skall l�sas 
    �vriga v�rden h�mtas fr�n fil 
    
    om filen saknas returnerar funktionen ERROR
    annars returneras TRUE
    
    kontroll av f�ltl�ngder g�rs. 
    Om inte inl�st f�lt f�r plats kapas efterf�ljande tecken.
    returv�rdet s�tts d� till FALSE

  
    */
    
{

	int i,j ;

char filename[128] ;    /* filnamn f�r fil som inneh�ller ett inl�gg */
char string[1024];       /* rad i meddelande */
struct _title title ;

FILE *fp ;

if ( !getTitle ( category_id, subject_id, object_id, &title ))
{
  errorHTML ("Document not found\n") ;
  exit(0) ;
} 

document->category = category_id ;
document->subject = subject_id ;
document->id = object_id ;
document->owner = title.owner ;
document->security = title.security ;

strcpy ( document->title, title.name ) ;
strcpy ( document->author, title.author ) ;
strcpy ( document->email, title.email ) ;
strcpy ( document->created, title.created ) ;
strcpy ( document->company, title.company ) ;

    /* �ppna filen f�r l�sning */

if ( strcmp ( EDITION, "Limited"))
  sprintf( filename, "%s/message-%d-%d-%d.dat", DB_PATH, 
               document->category,  document->subject, document->id ) ; 
else
  sprintf( filename, "%s/message-%d-%d.dat", DB_PATH, document->subject, document->id ) ; 

if (( fp = fopen ( filename, "r" )) == NULL )
   {
    sprintf ( string, "Dokumentet %s kan ej l�sas.", filename ) ;
    errorHTML ( string) ;
    return ( -1 ) ;
   }


  /* skriv inl�gg till fil fil 
  rad 1	l�nk -> dokumentnummer -> version -> f�rfattare -> kontrollerad
-> godk�nd -> distrubueras
rad 2->	beskrivande text
   */
  /* l�s inl�gg fr�n fil */

if ( fgets ( string, 1024, fp ) == NULL )
{
    sprintf ( string, "Dokumentet %s kan ej l�sas.", filename ) ;
    errorHTML ( string) ;
    return ( -1 ) ;
   }


for (i=0; i<(int)strlen(string);i++ ) 
{ if (  string[i] == '\t' ) 
    break ;
  document->href[i] = string[i] ;

}
 
document->href[i] = (char) NULL ;
i ++ ;

for (j=0; i<(int)strlen(string);i++,j++ ) 
{ if (  string[i] == '\t' ) 
    break ;
  document->filename[j] = string[i] ;

}

document->filename[j] = (char) NULL ;
i ++ ;

for (j=0; i<(int)strlen(string);i++,j++ ) 
{ if (  string[i] == '\t' ) 
    break ;
  document->version[j] = string[i] ;

}

document->version[j] = (char) NULL ;
i ++ ;

  /* hoppa �ver f�lt */
for (j=0; i < (int)strlen(string);i++,j++ ) 
{ if (  string[i] == '\t' ) 
    break ;

}

i ++ ;

  
for (j=0; i< (int)strlen(string);i++,j++ ) 
{ if (  string[i] == '\t' ) 
    break ;
  document->verified[j] = string[i] ;

}

document->verified[j] = (char) NULL ;
i ++ ;

for (j=0; i< (int)strlen(string);i++,j++ ) 
{ if (  string[i] == '\t' ) 
    break ;
  document->approved[j] = string[i] ;

}

document->approved[j] = (char) NULL ;
i ++ ;

for (j=0; i< (int)strlen(string);i++,j++ ) 
{ if (  string[i] == '\t' ) 
    break ;
  document->distribute[j] = string[i] ;

}

document->distribute[j] = (char) NULL ;
i ++ ;
 
  /* l�gg till �vriga textrader */

document->text[0] = (char) NULL ;


while ( fgets ( string, 255, fp ) != NULL )
  strcat( document->text, string ) ;


fclose ( fp ) ;

return ( TRUE ) ;


}  /* end of readDocument */

/****



  ****/

updateDocument ( struct _document *document )

  /*
    
    Skriver �ver ett inl�gg till tv� olika filer

    news-?.dat som inneh�ller titel
    msg-?-?.dat som inneh�ller sj�lva inl�gget

    rad 1	
	l�nk -> dokumentnummer -> version -> f�rfattare -> kontrollerad -> godk�nd -> distrubueras

    rad 2
	beskrivande text

    filen news-?.id inneh�ller r�knare f�r n�sta lediga post
       
    En l�sfil news-?.lock anv�nds f�r att kontrollera s� att inga andra
    processer skriver till titelfilen, eller r�knarfilen
    
    funktionen returnerar TRUE om inl�ggets sparats ned
    vid filfel s� returneras ERROR
    om l�sningsfilen inte kan skrivas returneras FALSE 

  */

{



char filename[128] ;    /* filnamn f�r fil som inneh�ller ett inl�gg */
struct _title title ;

FILE *fp, *lock ;

    /* s� l�nge denna filen �r �ppna kan inga andra inl�gg i gruppen g�ras */

if ((lock = lockSubject ( document->category, document->subject )) == NULL ) 
  return ( FALSE ) ;

title.id = document->id ;
title.category = document->category ;
title.subject = document->subject ;
title.owner = document->owner ;
title.security = document->owner ;
title.indent = document->indent ;     
title.object = 3 ;                     /* dokument */

strcpy ( title.name, document->title ) ;
strcpy ( title.email, document->email ) ;
strcpy ( title.author, document->author ) ;
strcpy ( title.created, document->created ) ;
strcpy ( title.company, document->company ) ;

updateTitle ( title ) ;

    /* �ppna filen f�r skrivning av �vrig artikeldata */

if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/message-%d-%d-%d.dat",DB_PATH,  document->category, document->subject, document->id  ) ;
else 
  sprintf( filename, "%s/message-%d-%d.dat", DB_PATH, document->subject, document->id ) ; 

if (( fp = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej skrivas.") ;
    return ( -1 ) ;
   }

  /* skriv inl�gg till fil fil 
  rad 1	l�nk -> dokumentnummer -> version -> f�rfattare -> kontrollerad
-> godk�nd -> distrubueras
rad 2->	beskrivande text
   */
fprintf( fp, "%s\t%s\t%s\t%s\t%s\t%s\t%s\n", document->href, document->filename, 
						document->version, document->author, 
						document->verified, document->approved,
						document->distribute ) ;

fputs ( document->text, fp ) ;


  /* st�ng l�sningsfilen f�r id */
  /* st�ng l�sningsfilen f�r id */    

unlockSubject ( lock ) ;

return ( TRUE ) ;

} /* end of updateDocument () */



/*****************************************************************/

void printDocumentForm ( struct _document document )

{

time_t today ;
	
char sCategory[32] ;
char sNewsgroup[32] ;

struct _subject subject ;

if ( strcmp ( EDITION, "Limited" ))
  getCategoryName ( document.category, sCategory ) ;

if ( document.subject > 0 )
  getSubjectName ( document.category, document.subject, sNewsgroup ) ;


if ( document.id != 0 )
  printf("<H2>�ndra dokument</H2>\n") ;
else
  printf("<H2>Nytt dokument</H2>\n") ;

printf("<FORM method=\"post\" action=\"%s/document%s\">\n", CGI_PATH, EXECUTABLE ) ; 


if ( document.id  != 0 )   /* befintligt dokument */ 
{
 printf( "<input object=hidden name=method value=\"update\"> \n") ;
 printf( "<input object=hidden name=id value=\"%d\"> \n", document.id ) ;
}
else
  printf("<input object=hidden name=method value=\"insert\"> \n") ;


if ( strcmp ( EDITION, "Limited"))
  printf( "<input object=hidden name=category  value=\"%d\"> \n", document.category ) ;

printf("<table border=0>\n") ;

if ( strcmp ( EDITION, "Limited"))
 {
  printf("<tr>") ;
  printf("<td>Kategori: </td>") ;
  printf("<td> %s", sCategory ) ;

  printf("</td>") ;
  printf("</tr>\n<tr>") ;
 }

printf("<td>�mne: </td>") ;
printf("<td>" ) ;

subject.category = document.category ;

if ( document.subject == 0 )
  {
    selectSubject ( &subject,"ALL","DATE", NULL ) ; /* lds vnskade nyhetsgrupper fren fil */
    SubjectListBoxHTML ( &subject ) ; 
  } 
else
  {
   printf("%s", sNewsgroup ) ;
   printf("<input object=hidden name=subject value=\"%d\">", document.subject ) ;
  } 

printf("</td></td></tr>\n") ;

printf("<tr><td>Rubrik:</td><td>") ;
TextBoxHTML ( "title", document.title, 25 ) ;
printf("</td>") ;

printf("<td>Spr&aring;k: </td><td> ") ;
printf("<SELECT name=\"language\">") ;
printf("<OPTION value=\"-1\">") ;
printf("<OPTION value=\"1\">%s","Engelska" ) ;
printf("<OPTION value=\"46\">%s", "Svenska" ) ;
printf("</SELECT></td>") ;

printf("<td>S&auml;kerhetsklass: </td><td>") ; 
printf("<SELECT name=\"infoclass\"> ") ;
printf("<OPTION value=\"-1\">") ;
printf("<OPTION value=\"0\">%s", "Bazar" ) ;
printf("<OPTION value=\"1\"> %s", "Public" ) ;
printf("<OPTION value=\"2\"> %s", "Internal" ) ;
printf("<OPTION value=\"3\">%s","Confidential") ;
printf("<OPTION value=\"4\">%s","Secret") ;
printf("<OPTION value=\"5\">%s","Private") ;
printf("</SELECT></td>") ;

printf("<tr>") ;
printf("<tr><td>") ;
printf("Dokument: </td><td> ") ; 
printf("<input object=\"file\" name=\"file\" value=\"\">") ;
printf("</td>") ; 
printf("</tr>\n") ;
printf("</table>\n") ;

printf("<table>\n") ;
printf("<tr><td>") ;
printf("Status: </td><td>") ;
printf("<SELECT name=\"lifecycle\"> ") ;
printf("<OPTION value=\"-1\">") ;
printf("<OPTION value=\"1\">%s", "Template" ) ;
printf("<OPTION value=\"2\"> %s", "Draft" ) ;
printf("<OPTION value=\"3\"> %s", "Preliminary" ) ;
printf("<OPTION value=\"4\">%s","Checked") ;
printf("<OPTION value=\"5\">%s","Approved") ;
printf("<OPTION value=\"6\">%s","Published") ;
printf("<OPTION value=\"7\">%s","Superseeded") ;
printf("<OPTION value=\"8\">%s","Withdrawn") ;
printf("<OPTION value=\"9\">%s","Archived") ;
printf("<OPTION value=\"10\">%s","Deleted") ;
printf("</SELECT>") ;
printf("</td><td>Version:</td><td>") ;
TextBoxHTML ("version", document.version,12 ) ;

time ( &today ) ;     /* h�mta aktuell tid */
strftime( document.created, 26,"%Y-%m-%d %H:%M:%S", localtime(&today) ) ;

printf("</td>\n") ;
printf("<td>Datum: </td><td>\n") ;
TextBoxHTML ("date",document.created,12 ) ;
printf("</td></tr>") ;
printf("</table>\n") ;

printf("<table>\n") ;
printf("<tr><td>F&ouml;rfattare:<td>") ;
TextBoxHTML ( "author", document.author, 25 ) ;
printf("</td>") ;

printf("<td valign=\"top\">E-post:<td>") ;
TextBoxHTML ( "email", document.email, 25 ) ;
printf("</td></tr>") ;

printf("</table>\n") ;
printf("<table>") ;

printf("<tr><td>\n" ) ;
printf("Kontrollerad: <input object=\"checkbox\" name=\"checked\" value=\"true\">\n") ;
printf("Godk&auml;nd: <input object=\"checkbox\" name=\"approved\" value=\"true\">\n") ;
printf("Publicerad: <input object=\"checkbox\" name=\"published\" value=\"true\">\n") ;
printf("</td></tr>\n" ) ;

printf("</table>\n") ;
printf("<table>") ;

/*
if ( checked )
 {
   printf("<tr><td>Kontrollerad:<td>") ;
   TextBoxHTML ( "verified by ", document.verified, 25 ) ;
   printf("</td></tr>") ;
  }
  
if ( approved ) 
 { 
  printf("<tr><td>Godk�nd:<td>") ;
  TextBoxHTML ( "approved by", document.approved, 25 ) ;
  printf("</td></tr>") ;
 }

if ( published ) 
 {
   printf("<tr><td>Publicerad av<td>") ;
   TextBoxHTML ( "verified by ", document.published, 25 ) ;
   printf("</td></tr>") ;
  }

*/

printf("</table>\n") ;
printf("<table>") ;
printf("<tr>") ;
printf("<td>Omr&aring;de: </td><td><SELECT name=\"taxanomy-1\">") ;
printf("<OPTION value=\"-1\">") ;
printf("<OPTION value=\"1\">Fotografering") ;
printf("<OPTION value=\"2\">Systemutveckling") ;
printf("<OPTION value=\"3\">Resor") ;
printf("<OPTION value=\"4\">Skid&aring;kning") ;
printf("<OPTION value=\"5\">Systemutveckling") ;
printf("<OPTION value=\"6\">Sportdykning") ;
printf("<OPTION value=\"7\">Sportfiske") ;
printf("<OPTION value=\"8\">Matlagning") ;
printf("</SELECT></td>") ;

printf("<td>Ort: </td><td><SELECT name=\"taxanomy-2\">") ;
printf("<OPTION value=\"-1\">") ;
printf("<OPTION value=\"1\">Stockholm") ;
printf("<OPTION value=\"2\">Sundsvall") ;
printf("<OPTION value=\"3\">Falkenberg") ;
printf("<OPTION value=\"4\">G&ouml;eborg") ;
printf("<OPTION value=\"5\">Helsingborg") ;
printf("<OPTION value=\"6\">&Ouml;stersund") ;
printf("</SELECT></td>") ;

printf("<td>Kunskapsniv&aring; </td><td><SELECT name=\"taxanomy-3\">") ;

printf("<OPTION value=\"-1\">") ;
printf("<OPTION value=\"1\">Beginner") ;
printf("<OPTION value=\"2\">Intermediate") ;
printf("<OPTION value=\"3\">Advanced") ;
printf("<OPTION value=\"4\">Expert") ;
printf("<OPTION value=\"5\">Guru") ;
printf("</SELECT></td>") ;
printf("<br>") ;
printf("</tr>") ;
printf("</table>") ;

printf("<table>\n") ;

printf("<tr><td valign=\"top\">Beskrivning:</td></tr><tr><td>") ;
EditBoxHTML ( "text", document.text, 5, 65 ) ;
printf("</td></tr>") ;

printf("<tr><td>" ) ;
printf("<input object=submit value=\"Spara \">") ;
printf("<input object=reset value=\"Avbryt\">") ;
printf ("</td></tr></table>") ;
printf ("</FORM>" ) ;
 
}

deleteDocument ( struct _document *document )

     /*


    news-?.dat som inneh�ller titel
    msg-?-?.dat som inneh�ller sj�lva inl�gget

    filen news-?.id inneh�ller r�knare f�r n�sta lediga post
       
    En l�sfil news-?.lock anv�nds f�r att kontrollera s� att inga andra
    processer skriver till titelfilen, eller r�knarfilen
    
    funktionen returnerar TRUE om inl�ggets sparats ned
    vid filfel s� returneras ERROR
    om l�sningsfilen inte kan skrivas returneras FALSE 
  

*/

{

struct _title title ;

FILE *lock ;


    /* s� l�nge denna filen �r �ppna kan inga andra inl�gg i gruppen g�ras */

if ((lock = lockSubject ( document->category, document->subject )) == NULL ) 
  return ( FALSE ) ;

    /* radera inl�gg */

title.id = document->id ;
title.category = document->category ;
title.subject = document->subject ;
title.owner = document->owner ;

deleteTitle ( title )  ;

unlockSubject ( lock ) ;

return ( TRUE ) ;

}

/*

 Skapa l�nk best�ende av virtual path och filnamn 
 virtual path h�mtas fr�n filen virtual.dat
 
*/

int  createDocumenthref ( struct _document *document )

{

int i,j ;

char filename [128] ;
char string[512] ;
char tmp[128] ;

struct _category category ;

FILE *fp ;

sprintf( filename, "%s/virtual.dat", DB_PATH ) ;

if (( fp = fopen ( filename, "r" )) == NULL )
   {
    errorHTML ("Virtual path saknas.") ;
    return ( -1 ) ;
   }

 /* om r�tt id s� l�st virtual path */

while ( fgets ( string, 512, fp ) != NULL )
 {      
   	for (i=0;i<(int)strlen(string);i++)
{ 
      if ( string[i] == '\t' )
		break ;
      tmp[i] = string [i] ;
	}
    i++ ;

    category.id = atoi ( tmp ) ;   /* categori som f�rsta f�lt innan f�ltavskiljare */
  
   if ( category.id == document->category)
   {
    for (j=0;i<(int)strlen(string);j++,i++)
	{
      if ( string[i] == '\t' )
		break ;
      category.path[j] = string[i] ;
	}
    i++ ;
    category.path[j] = (char) NULL ;
	
	for (j=0;i<(int)strlen(string);j++,i++)
	{
      if ( string[i] == '\t' )
		break ;
      category.server[j] = string[i] ;
	}
    i++ ;
    category.server[j] = (char) NULL ;
	break ;

  }  /* if id == */
  
}  /* end of while */

fclose ( fp ) ;

sprintf( document->href, "%s%s/%s", category.server, category.path, document->filename ) ;

return ( TRUE ) ;

}


/* letar upp r�tt dokument */

findDocument ( struct _document *document, struct _title *title ) 


{

int indent ;    /* r�knare f�r niv� p� inl�gg */
int counter ;

struct _title *first ;  /* f�rsta rubriken */

first = title ;
counter = 0 ;  
indent = 0 ;

while ( title != NULL ) 
 {

  if ( title->id == document->id )   /* r�tt rubrik */
   {
     strcpy ( document->title, title->name ) ;
     strcpy ( document->author, title->author ) ;
     strcpy ( document->email, title->email ) ;
     strcpy ( document->created, title->created ) ;
     break ;
     
   }          
  title = title->next ;
  counter ++ ; 
 
 }

title = first ;

return ( TRUE ) ;

}



/********************************************************


 Intranet
 
 Program:  %M%
 Version:  %I%
 Date:     %D% 

 Show titles of text that contains search text

*********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SELECT 0
#define INSERT 1
#define UPDATE 2
#define DELETE 3
#define EDIT 4
#define LIST 5

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
     expandNewsgroup

       
     printConferanceMenuHTML   - skriv ut tillg�ngliga menyval
     printSubjectHTML          - skriv ut valda rubriker

  */

int ExpandSearch ( struct _CGIarg *CGIarg, int *category_id, int *subject_id, int *object_id, char *scope, char *name, char *author, char *keyword, char *text, char *sort, char *cursor ) ;

int SearchIntra ( struct _user user, struct _title *title, int category_id, int subject_id, int object_id, char *name, char *author, char *keyword, char *text, char *sort, char *cursor ) ;

void printSearchFormHTML ( char *scope ) ;
int search_file ( char *filename, char *text ) ;

 /* huvudprogram  search */

main ( int argc, char *argv[])


{
char path[255] ;
int method ;
int category_id ;
int subject_id ;
int object_id ;
int type_id ;

char scope[255] ;
char name[255] ;
char author[255] ;
char keyword[255] ;
char text[255] ;
char sort[255] ;
char cursor[255] ;
char page[255] ;


char *instring ;             /* inl�st teckenstr�ng fr�n formul�r */

struct _CGI CGI ;            /* CGI environment */
struct _CGIarg CGIarg ;      /* lista med varibelnamn och v�rden fr�n formul�r */
struct _user user ;          /* user profile */ 
struct _title title ;        /*              */

  
/* l�s in CGI environment */

strcpy ( path, argv[0] ) ;
InitIntra ( path) ;

InitCGI () ;
GetCGI ( &instring, &CGI ) ;    

  /* konvertera v�rden fr�n formul�r */

bURLencode ( instring, &CGIarg ) ;
ExpandMethod ( &CGIarg, &method ) ;    
ExpandSearch ( &CGIarg, &category_id, &subject_id, &object_id, scope, name, author, keyword, text, sort, cursor ) ;

findUser ( CGI.remote_user, &user ) ;
 
  /* visa ol�sta rubriker */
                       
if ( method == SELECT )
  {                     
                              
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */

     /* l�s nya rubriker */  
   
   SearchIntra ( user, &title, category_id, subject_id, object_id, name, author, "", text, "","") ;
   printTitleHTML ( category_id, subject_id, user, &title ) ;

   catHTML ( FOOTER ) ;           /* skriv ut en sidfot i HTML        */ 
  }

if ( method == EDIT )
  {                     
                              
   catHTML ( HEADER ) ;           /* skriv ut ett sidhuvud i HTML     */

   /* skriv ut meny f�r konferanssystem */
   if ( !FRAMES )
     exit(0) ;  

     printSearchFormHTML ( scope ) ; 

   catHTML ( FOOTER ) ;           /* skriv ut en sidfot i HTML        */
  }


return ( TRUE ) ;                    


}  /* end of main () */




SearchIntra ( struct _user user, struct _title *title, int category_id, int subject_id, int object_id, char *name, char *author, char *keyword, char *text, char *sort, char *cursor )

   /* subject - l�nkad lista med aktuella rubriker
      show    - UNREAD, visa alla inl�gg
      sort    - DATE, sortera efter datum
      cursor  - NULL
         
   */

{


int counter ;           /* antal l�sta poster */
int subjects ;          /* vilken nyhetsgrupp som avses */
int categories ;        /* vilka rubriker som finns */

char filename[128] ;    /* filnamn f�r fil som inneh�ller ett inl�gg */
char string[512];       /* rad f�r rubrik */

char sName[40] ;

int skip ;                 /* hoppa �ver posten */
int hits ;
               
FILE *fp, *lock ;

struct _title *first_title ;
struct _title *new_title ;

counter = 0 ;

filename[0] = (char) NULL ;
skip = FALSE ;

if (( first_title = malloc (sizeof(struct _title))) == NULL )

	 { 
	   printf ("No memory avaliable for allocation\n") ;
      exit(1) ;
	 } 

first_title = title ;            /*  Set memory adress to first title in list */  
title->next = NULL ;   /*  This is the last title so far */          
 
/* l�s antal katagorier */ 
 
  categories = getLastCategoryId () ;
    
  for (category_id=1;category_id<=categories;category_id++)   /* f�r varje katagori */
   {
  
   /* l�s antal �mnen f�r aktuell kategori */
    subjects = getLastSubjectId ( category_id) ;

    if ( subjects == 0 )
      { continue ; }
 
    for (subject_id=1;subject_id<=subjects;subject_id++)
     {
    
      /* Om valt �mne saknas skall man hoppa �ver detta �mne */
      if ( !getSubjectName ( category_id,subject_id,sName ))
       { continue ;}

 
  /* s�tt v�rden f�r aktuell kategori / �mne */


  /* Om l�sfilen inte g�r att �ppna skall man inte forts�tta */

if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/news-%d-%d.lck", DB_PATH, category_id, subject_id ) ; 
else
  sprintf( filename, "%s/news-%d.lck", DB_PATH, subject_id ) ; 

if (( lock = fopen ( filename, "w" )) == NULL )
   {
    sprintf ( string, "Rubriker %s kan ej l�sas.", filename ) ;
    errorHTML ( string ) ;
    return ( -1 ) ;
   }

    /* �ppna filen f�r l�sning */

if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/news-%d-%d.dat", DB_PATH, category_id, subject_id ) ; 
else
  sprintf( filename, "%s/news-%d.dat", DB_PATH, subject_id ) ; 

if (( fp = fopen ( filename, "r" )) == NULL )
   {
    sprintf ( string, "Rubriker %s kan ej l�sas.", filename ) ;
    errorHTML ( string ) ;
    return ( -1 ) ;
   }

  /* l�s rubriker fr�n fil */

while ( fgets ( string, 255, fp ) != NULL )
  {                                              

   title->category = category_id ;
   title->subject = subject_id ;
   readTitle ( string, title ) ;     
   
   /* Skall posten vara med ? */
 
   skip = TRUE ;
   hits = 0 ;

 
   if ( strlen (name) > 0 )
     if ( strstr ( title->name, name ) != NULL )
       { hits = hits + 1 ; }
 
  if ( strlen (author) > 0 )
     if ( strstr ( title->author, author ) != NULL )
       { hits = hits + 1 ; }

   /*
   if ( strlen (keyword) > 0 )
     if ( strstr ( title->keyword, keyword ) != NULL )
       { hits = hits + 1 ; }
   */

  /* om s�kord finns i rubriken skall antal hits �kas med 2 */
 
   if ( strlen (text) > 0 )
     if ( strstr ( title->name, text ) != NULL )
       { hits = hits + 2 ; }
	 
     /* om s�kord finns i texten skall antalet hits �kas med 1 */
  
     if ( strcmp( EDITION, "Limited"))
       sprintf( filename, "%s/message-%d-%d-%d.dat",DB_PATH,  title->category, title->subject, title->id  ) ;
     else 
       sprintf( filename, "%s/message-%d-%d.dat", DB_PATH, title->subject, title->id ) ;
  
  
   /* if search text then look into each file */
   
   if ( strlen (text) > 0 )
     hits = hits + search_file ( filename, text ) ;

	 
   /* om tr�ffar finns s� skall rubriken skrivas ut */
 
   if ( hits > 0 ) 
	 skip = FALSE  ;    
  
  
  if ( skip == FALSE  )                                /*  posten skall l�sas  - f�r �ver till l�nkad lista*/
  {
   counter ++ ;

   if (( new_title = malloc (sizeof(struct _title))) == NULL )

	 { 
	   printf ("No memory avaliable for allocation\n") ;
      exit(1) ;
	 } 
    
	  new_title->next = NULL  ;       /* */
      title->next = new_title ;    /* */
      title = new_title ;              /* Set new title to previous title */ 

  }  /* end of if skip == FALSE */ 
 else
 {
	 title->name[0] = (char) NULL ;
	 title->category = 0 ;
	 title->subject = 0 ;
 } 
}   /* end of while */

fclose ( fp ) ;
fclose ( lock ) ;

   } 

 }  /*  */

title = first_title ;

return ( counter ) ;
 
} /* end of SearchIntra */


int ExpandSearch ( struct _CGIarg *CGIarg, int *category_id, int *subject_id, int *object_id, char *scope, char *name, char *author, char *keyword, char*text, char *sort, char *cursor ) 

  /* 
      Funktionen �vers�tter argument fr�n CGI-script till en post i en l�nkad lista
      
      Om inga fel intr�ffar returneras TRUE
      Om v�rde �verskrider min eller maxv�rde s� returnerar funktionen FALSE
      Om n�gon annan typ av fel intr�ffar s� returneras ERROR
      
      CGIarg innh�ller en l�nkad lista med variabelnamn och v�rden
      
      *article inneh�ller en pekare till en post med v�rden. 
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

scope[0] = (char) NULL ;
name[0] = (char) NULL ;
keyword[0] = (char) NULL ;
author[0] = (char) NULL ;
text[0] = (char) NULL ;


while ( CGIarg != NULL ) 
 {

  size = strlen ( CGIarg->value ) ;

  if ( !strncmp("category", CGIarg->name, 8 )) {
    *category_id = atoi ( CGIarg->value ) ;
   }
   
  if ( !strncmp("author", CGIarg->name, 6 )) {
      strncpy ( author, CGIarg->value, 255 ) ;
	author[size] = (char) NULL ;
   }
  
  if ( !strncmp("text", CGIarg->name, 6 )) {
    *object_id = atoi ( CGIarg->value ) ;
   }

  if ( !strncmp("subject", CGIarg->name, 7 )) {
    *subject_id = atoi ( CGIarg->value ) ;
   }

  if ( !strncmp("title", CGIarg->name, 5 )) {
    strncpy ( name, CGIarg->value, 255 ) ;
	name[size] = (char) NULL ;
   }

 if ( !strncmp("scope", CGIarg->name, 5 )) {
    strncpy ( scope, CGIarg->value, 255 ) ;
	scope[size] = (char) NULL ;
   }


 if ( !strncmp("keyword", CGIarg->name, 7 )) {
    strncpy( keyword, CGIarg->value,255 ) ;
   keyword[size] = (char) NULL ;
   }

   if ( !strncmp("text", CGIarg->name, 4 )) {
    strncpy( text, CGIarg->value,255 ) ;
   text[size] = (char) NULL ;
   }

  CGIarg = CGIarg->next ;
 }

CGIarg = first ;
/*free ( first ) ;*/

return ( TRUE ) ;

}   /* end of ExpandSearch */


/*  */
void printSearchFormHTML ( char *scope ) 

{

struct _category category ;

printf("<H2>S&ouml;k %s</H2>\n", scope ) ; 

printf("<FORM method=\"post\" action=\"%s/search%s\">\n", CGI_PATH, EXECUTABLE ) ; 
printf("<input type=hidden name=method value=\"select\"> \n") ;

printf("<table border=0>\n") ;


printf("<tr>") ;
/*

printf("<td>Kategori:</td>") ;

*/

printf("<td>") ;

/*
selectCategory ( &category,"ALL","DATE", NULL ) ; 
CategoryListBoxHTML ( &category ) ;
*/

printf("</td></tr>\n") ;


if ( !strncmp(scope, "text",4 ))
 {
   printf("<tr><td>Text:</td><td valign=\"top\">") ;
   TextBoxHTML ("text","", 25 ) ;
   printf("</td></tr>") ; 
  } 

if ( !strncmp(scope, "title", 5 ))
 {
   printf("<tr><td>Rubrik:</td><td valign=\"top\">") ;
   TextBoxHTML ("title","", 25 ) ;
   printf("</td></tr>") ; 
  } 

if ( !strncmp(scope, "author",6 ))
 {
   printf("<tr><td>F&ouml;rfattare:</td><td valign=\"top\">") ;
   TextBoxHTML ("author","", 25 ) ;
   printf("</td></tr>") ; 
  } 

if ( !strncmp(scope, "user",4 ))
 {
   printf("<tr><td>Anv&auml;ndare:</td><td valign=\"top\">") ;
   TextBoxHTML ("user","", 25 ) ;
   printf("</td></tr>") ; 
  } 

printf("<tr><td></td><td>" ) ;
printf("<input type=submit value=\"Forts&auml;tt \">") ;
printf("<input type=reset value=\"Avbryt\">") ;
printf ("</td></tr></table>") ;
printf ("</FORM>" ) ;
}

/**/
 
int search_file ( char *filename, char *text ) 

{

char string[4096] ;
char key[255] ;

FILE *fp  ;
	
int i ;

int hits ;

hits = 0 ;

if (( fp = fopen ( filename, "r" )) == NULL )
	return ( ERROR ) ; 

 /* �vers�tter efters�kt text till versaler */

key[0]= (char) NULL ;

for (i=0;i<(int)strlen(text);i++ )
  key[i]= toupper(text[i]) ;

key[i] = (char) NULL ;

 /* s�ker efter f�rsta f�rekomst av text / rad */

while ( fgets ( string, 4096, fp ) != NULL )
 {
  
  /* �vers�tter inl�st textenstr�ng till versaler */

  for (i=0;i<(int)strlen(string);i++ )
    string[i] = toupper(string[i] ) ;
	
   /* om inte ERROR s� finns text */
  if ( strstr ( string, key ) != NULL )
    hits ++ ; 
 
 }  /* end of while */

fclose ( fp ) ;

return ( hits ) ;

}
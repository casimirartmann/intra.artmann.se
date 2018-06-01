/********************************************************

 Konferanssystem
 
 Program:  %M%
 Version:  %I%
 Date:     %D% 

 Visar alla nya inl�gg i en RSS feed

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

void printAllTitleRSS ( struct _user user, struct _title *title ) ;
void printRSSHeader () ;
void printRSSFooter () ; 
   /* f�ljande funktioner anropas  */
     
  /* expandMethod
     expandNewsgroup

       
     printConferanceMenuHTML   - skriv ut tillg�ngliga menyval
     printSubjectHTML          - skriv ut valda rubriker

  */



 /* huvudprogram  newsgroup */

main ( int argc, char *argv[])


{
char path[255] ;
int method ;
int scope ;
int counter ;
char *instring ;             /* inl�st teckenstr�ng fr�n formul�r */
struct _CGI CGI ;            /* CGI environment */
struct _CGIarg CGIarg ;      /* lista med varibelnamn och v�rden fr�n formul�r*/
struct _user user ;          /* data f�r en anv�ndare */ 
struct _title *title ;  

/* l�s in CGI environment */

strcpy ( path, argv[0] ) ;
InitIntra ( path) ;

/* GetCGI ( &instring, &CGI ) ; */   

  /* konvertera v�rden fr�n formul�r */

/* bURLencode ( instring, &CGIarg ) ; */
/* ExpandMethod ( &CGIarg, &method ) ; */  
/* ExpandRSS ( &CGIarg, &scope ) ; */

findUser ( "", &user ) ;
 
  /* visa ol�sta rubriker */
                
    printRSSHeader () ;           /* skriv ut ett sidhuvud i HTML/RSS     */

     /* exit if not using frames */

   if ( !FRAMES )
     exit(0) ;

   /* read new titles */
  
   title = malloc ( sizeof ( struct _title )) ; 
   
   counter = SelectInbox ( user, title, "NEW", "DATE", "") ;
   
   printAllTitleRSS ( user, title ) ; /* skriv ut rubriker som HTML-kod f�r RSS sida */
   
   free ( title ) ;

    printRSSFooter () ;            /* skriv ut en sidfot i HTML/RSS        */

return ( TRUE ) ;                    


}  /* end of main () */

/********************************************

 **/

int ExpandRSS ( struct _CGIarg *CGIarg, int *scope ) 

{

struct _CGIarg *first ;

  /* spara en pekare till f�rsta argumentet i listan */ 
  
first = malloc ( sizeof( *CGIarg )) ; 
first = CGIarg ; 

*scope = -1 ;

while ( CGIarg != NULL )
 {
  if ( !strncmp("scope", CGIarg->name, 5 ))
   {
    if ( !strncmp ( "new", CGIarg->value,3 ))
      *scope = 1 ;
    if ( !strncmp ( "unread", CGIarg->value,6 ))
      *scope = 2 ;
    if ( !strncmp ( "all", CGIarg->value,3 ))
      *scope = 3 ;
    if ( !strncmp ( "read", CGIarg->value,4 ))
      *scope = 4 ;  

    if ( *scope >= 0 )    /* giltig metod */
      break ;
   }
    
  CGIarg = CGIarg->next ;
 }

CGIarg = first ;

/*free ( first ) ;*/

if ( *scope != -1 ) 
  return ( TRUE ) ;
else
  return ( FALSE ) ;
    
}   /* end of method */


SelectInbox ( struct _user user, struct _title *title, char *show, char *sort, char *cursor )

   /* subject - l�nkad lista med aktuella rubriker
      show    - UNREAD, visa alla inl�gg
      sort    - DATE, sortera efter datum
      cursor  - NULL
         
   */

{

int category_id ;               /* r�knare f�r str�ngar */
int subject_id ; 
int counter ;           /* antal l�sta poster */
int groups ;            /* vilken nyhetsgrupp som avses */
int categories ;        /* vilka rubriker som finns */

char filename[128] ;    /* filnamn f�r fil som inneh�ller ett inl�gg */
char string[255];       /* rad f�r rubrik */

char sName[40] ;

int skip ;                 /* hoppa �ver posten */
               
FILE *fp, *lock ;

struct _title cur_title ;    /* inl�st post */

struct _title *first_title ;
struct _title *new_title ;
struct _title *ptr_title ;


filename[0] = (char) NULL ;
skip = FALSE ;
counter = 0 ;
title->next = NULL ;
first_title = title ;
         
/* l�s antal katagorier */ 
 
  categories = getLastCategoryId () ;

  for (category_id=1;category_id<=categories;category_id++)   /* f�r varje katagori */
   {
  
   /* l�s antal �mnen f�r aktuell kategori */
    groups = getLastSubjectId ( category_id ) ;

    if ( groups == 0 )
     continue ;

    for (subject_id=1;subject_id<=groups;subject_id++)
     {
    
      /* Om valt �mne saknas skall man hoppa �ver detta �mne */
      if ( !getSubjectName ( category_id,subject_id,sName ))
        continue ;
     
 
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

    cur_title.created [0] = (char) NULL ;  
    readTitle ( string, &cur_title ) ; 
   /* Skall posten vara med ? */

   skip = TRUE ;

  
  if   ( !strncmp (show,"NEW",3))
    {
     if (  difftoday_str (  cur_title.created  ) <=  (double) ( 60 * 60 * 24 * DAYS ))
       skip = FALSE ;
    }

 if   ( !strncmp (show,"UNREAD",6))
    {
     if (  difftoday_str (  cur_title.created  ) <=  (double) ( 60 * 60 * 24 * DAYS ))
       skip = FALSE ;
       } 

  if ( !strncmp (show, "ALL",3))
    skip = FALSE ;

  if ( skip == FALSE  )                                /*  posten skall l�sas  - f�r �ver till l�nkad lista*/
    {

     if ( counter >  0)                  /* First record in list */
       {
        ptr_title = malloc ( sizeof ( struct _title )) ;
        ptr_title->next = NULL ;            
        title->next = ptr_title ;                 
        title = ptr_title ;
       }
    
      strcpy ( title->name, cur_title.name ) ;
      strcpy ( title->email, cur_title.email ) ;
      strcpy ( title->created, cur_title.created ) ;
      strcpy ( title->author, cur_title.author ) ;
      strcpy ( title->company, cur_title.company ) ;
      strcpy ( title->href, cur_title.href ) ;
      title->category = category_id ;
      title->subject = subject_id ;
      title->id = cur_title.id ;
      title->indent = cur_title.indent ;
      title->security = cur_title.security ;
      title->owner = cur_title.owner ;
      title->object = cur_title.object ;   
      title->subclass = cur_title.subclass  ;

      
      counter ++ ;     
    }        /* end of skip == TRUE ; */


  } /* end of while */


fclose ( fp ) ;
fclose ( lock ) ;
 
   }

 }  /*  */

title = first_title ;

return ( counter ) ;
 
} /* end of SelectInbox */




void printRSSFooter()
{
printf("</rss>");
}


/*************************************************************************/



void printAllTitleRSS ( struct _user user, struct _title *title )

  /* skriver ut titel p� ett objekt f�ljt av f�rfattare
     om nytt inl�gg s� visas en icon
	 om ol�st inl�gg s� visas inl�gget med fet stil

	 titel l�nkas till objektet och f�r f�rfattaren till hans e-postadress
     upp till 10 subniv�er visas p� sk�rmen via indrag, 11:e niv� och
	 d�refter visas med samma indrag som niv� 10
     	 alla objektobjectr kan kommenteras, f�rutom typ 0 och 1.
	
	 objektobjectr som st�ds

     0 - l�nk
	 2 - artikel
	 1 - meddelande
	 3 - dokument
	 4 - e-post
	 5 - image
	 6 - sound
	 7 - video

    */

{

int indent ;    /* r�knare f�r niv� p� inl�gg */
int counter ;
int category_id ;
int subject_id ;
int categories ;
int groups ;
int first ;

char sName[40] ;

char sCategory[32] ;        /* */ 
char sSubject[32] ;       /* */
char sRemark[255] ;              /* */

char OBJECT [32] ;

struct _title *ptr_title ;  /* f�rsta rubriken */

ptr_title = title ;

counter = 0 ; 
indent = 0 ;                   /* huvudrubriker har niv� 1 */ 

printf("<UL>") ;
 
while ( title != NULL )
 {
    /* om n�sta �mne s� skall ny rubrik skrivas ut */
 
  if (( title->category != category_id ) || 
	  ( title->subject !=  subject_id ))
    first = TRUE  ;

if ( first == TRUE ) 
{ 

  /*
  if ( strcmp ( EDITION, "Limited" ))
  printf ("<H3>%s - %s</H3>\n", sCategory, sSubject ) ;
else
  printf ("<H3>%s</H3>\n", sSubject ) ;
  */

   
   first = FALSE ;
} ;

   /* best�m niv� p� inl�gg */

  if ( title->indent > 10 )
    title->indent =  10 ;
  if ( title->indent < 0 )
    title->indent = 0 ;

  if ( indent < title->indent )
    for (; indent < title->indent;indent++)     
       printf("<UL>") ;
  if ( indent > title->indent )
      for (; indent > title->indent;indent--)
         printf("</UL>") ;

  indent = title->indent ;

if ( strlen (title->name) > 0 )
  {

 if  ( title->object == -1 )    /* alias */   
     strcpy ( OBJECT, "alias") ;
  if ( title->object == 0 )    /* HTML-l�nk */   
     strcpy ( OBJECT, "folder") ;
  if ( title->object == 1 )    /*  */ 
	 strcpy ( OBJECT, "href") ;
  if ( title->object == 2 )    /* artikel i konferansystem */ 
	 strcpy ( OBJECT, "article") ;
  if ( title->object == 3)    /* meddelande  i konferansystem */ 
	 strcpy ( OBJECT, "message") ;
  if ( title->object == 5 )    /* document  i konferansystem */ 
	 strcpy ( OBJECT, "document") ;
  if ( title->object == 4 )    /* e-mail message */ 
	 strcpy ( OBJECT, "letter") ;
  if ( title->object == 6 )    /* image */ 
	 strcpy ( OBJECT, "image") ;
  if ( title->object == 7 )    /* audio */ 
	 strcpy ( OBJECT, "audio") ;
  if ( title->object == 8 )    /* video */ 
	 strcpy ( OBJECT, "video") ;
  if ( title->object == 9 )    /*  */ 
	 strcpy ( OBJECT, "publication") ;
  if ( title->object == 10 )
              strcpy ( OBJECT, "asset" ) ;

     if ( strcmp (EDITION,"Limited"))
	 {
      printf("<a href=\"%s/%s%s?method=edit&category=%d&group=%d&id=%d\"><img border=0 src=\"%s/%s.gif\"></a> <a href=\"%s/%s%s?method=select&category=%d&group=%d&id=%d\">%s</a> ",
		      CGI_PATH, OBJECT, EXECUTABLE, title->category, title->subject, title->id, ICON_PATH, OBJECT, CGI_PATH, OBJECT, EXECUTABLE, title->category,  title->subject, title->id, title->name ) ;
	  if ( strlen ( title->author ) > 0 )
		  printf("- <a href=\"mailto:%s\">%s</a>\n",
                  title->email, title->author ) ;
     }
	  else
	  {
        printf("<a href=\"%s/%s%s?method=edit&group=%d&id=%d\"><IMG SRC=\"%s/%s.gif\" border=0></a> <a href=\"%s/%s%s?method=select&group=%d&id=%d\">%s</a> ",
			    CGI_PATH, OBJECT, EXECUTABLE, title->subject, title->id, ICON_PATH, OBJECT, CGI_PATH, OBJECT, EXECUTABLE, title->subject, title->id, title->name ) ;
		if (( strlen ( title->author ) > 0 ) && ( strlen ( title->email) > 0 ))
		  printf("- <a href=\"mailto:%s\">%s</a>\n",
                  title->email, title->author ) ; 
	  }
 
  /* ny rubrik  */

  if ( difftoday_str (  title->created  ) <=  (double) ( 60 * 60 * 24 * DAYS ))
    printf ("<IMG SRC=\"%s/new.gif\"><br>\n", ICON_PATH ) ;
 else
  printf(" <br>\n") ;

  }

  title = title->next ;

 }

/*  S�tt r�tt niv� p� indentering */   

if ( indent > 0  )
    for (; indent > 0;indent--)     
       printf("</UL>") ;
 
if ( indent <  0  )
      for (; indent < 0 ;indent++)
         printf("<UL>") ;

 printf("</UL>") ;   /* ett sista avslutande som matcthar f�rsta */   

title = ptr_title ;

}  /* end of printAllTitleHTML */




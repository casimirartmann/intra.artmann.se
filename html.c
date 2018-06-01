
  /****************************************************************************************

   Program: %M%
   Version:  %I%
   Date:       %D%

   written by Casimir Artmann. 1996-1998
   
    printConferanceMenuHTML ()
    printNewsgroupHTML ()
    printSubjectHTML ()
    printArticleHTML ()

    printArticleForm ()
    printCommentForm ()
    printNewsgroupForm () 

  */


 /*
    skriver ut ur menyval för konferansystemet
    aktuella menyval är beroende på vilket kommando som getts,
    samt vilka värden som finns i skärmbilden

  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../cgi.h"
#include "../utils.h"
#include "intra.h"
#include "db.h"
#include "html.h"

/* Skriver ut aktuell informationsklass */

int printInformationClassHTML ( int iInformationClass )
{

printf ("<cite>(") ;

if ( iInformationClass == -1 )
  printf ("Undefined") ;
if ( iInformationClass == 0 )
  printf ("External") ;
if ( iInformationClass == 1 )
  printf ("Public") ;
if ( iInformationClass == 2 )
  printf("Internal");
if ( iInformationClass == 3 )
  printf("Confidential") ;
if ( iInformationClass == 4 )
  printf ("Secret") ;
if ( iInformationClass == 5 )
  printf ("Private") ;
  if ( iInformationClass == 6 )
  printf ("Personal") ;
  
printf (")</cite> \n") ;

}


/*
  List all categories within one server
  This function is not avaliable in Limited Edition

*/
void printCategoryHTML (  struct _user user, struct _category *category )

{
   
int counter ;

struct _category *first ;

first = category ;
counter = 0 ; 

printf("<P>") ;

while ( category != NULL )
 {
 
 if ( validUser ( user.id, category->security, SELECT, category->owner, category->group ) == TRUE) /* skall rader skrivas ut */   
  {
    
	if ( category->security == ERROR  ) 
	 printf ("<IMG SRC=\"%s/undefined.gif\"> ", ICON_PATH) ;
	
	 if ( category->security == EXTERNAL ) 
	 printf ("<IMG SRC=\"%s/external.gif\"> ", ICON_PATH) ;
	 
	if ( category->security == PUBLIC ) 
	 printf ("<IMG SRC=\"%s/public.gif\"> ", ICON_PATH) ;

	if ( category->security == INTERNAL ) 
	 printf ("<IMG SRC=\"%s/internal.gif\"> ", ICON_PATH) ;

	if ( category->security== CONFIDENTIAL ) 
	 printf ("<IMG SRC=\"%s/confidential.gif\"> ", ICON_PATH) ;

	if ( category->security >= SECRET ) 
      printf ("<IMG SRC=\"%s/secret.gif\"> ", ICON_PATH) ;

	printf("<a href=\"%s/subject%s?method=select&category=%d\">%s</a>",
                  CGI_PATH, EXECUTABLE, category->id, category->name  ) ;
    printf("<br>") ;
  }
  
  category = category->next ;
  counter ++ ;
 }

category = first ;

}   /* end of printCategoryHTML */


	/*

   List all subjects within a category


*/

void printSubjectHTML ( struct _user user, struct _subject *subject )

{
   
int counter ;
int category ;
char sCategory[32]  ;

struct _subject *first ;

first = subject ;
counter = 0 ; 

if ( strcmp ( EDITION, "Limited"))
  {
   category = subject->category ;
   getCategoryName ( category, sCategory ) ;
   printf("<H3>%s</H3>", sCategory ) ;
  }

printf("<p>") ;

while ( subject != NULL )
 {
  if ( validUser ( user.id, subject->security, SELECT, subject->owner, subject->group ) == TRUE ) /* skall rader skrivas ut */   
   {  
	if ( subject->security == ERROR  ) 
	 printf ("<IMG SRC=\"%s/undefined.gif\"> ", ICON_PATH) ;
	
	 if ( subject->security == EXTERNAL ) 
	 printf ("<IMG SRC=\"%s/external.gif\"> ", ICON_PATH) ;
	 
	if ( subject->security == PUBLIC ) 
	 printf ("<IMG SRC=\"%s/public.gif\"> ", ICON_PATH) ;

	if ( subject->security == INTERNAL ) 
	 printf ("<IMG SRC=\"%s/internal.gif\"> ", ICON_PATH) ;

	if ( subject->security== CONFIDENTIAL ) 
	 printf ("<IMG SRC=\"%s/confidential.gif\"> ", ICON_PATH) ;

	if ( subject->security >= SECRET ) 
      printf ("<IMG SRC=\"%s/secret.gif\"> ", ICON_PATH) ;

 if ( category > 0 )
    printf ("<a href=\"%s/title%s?method=select&category=%d&subject=%d\">%s</a>",
                 CGI_PATH, EXECUTABLE, category, subject->id, subject->name ) ;
  else
    printf ("<a href=\"%s/title%s?method=select&subject=%d\">%s</a>", CGI_PATH, EXECUTABLE, subject->id, subject->name ) ;
	
  printf("<br>") ;
}
  subject = subject->next ;
  counter ++ ;
 }


subject = first ;


}   /* end of printNewsgroupHTML */

    /* listar alla rubriker på alla titlar i en
       nyhetsgrupp som länkar till artiklarna */

void printTitleHTML ( int category_id, int subject_id, struct _user user, struct _title *title )

  /* skriver ut titel på ett objekt följt av författare
     om nytt inlägg så visas en icon
	 om oläst inlägg så visas inlägget med fet stil

	 titel länkas till objektet och för författaren till hans e-postadress
     upp till 10 subnivåer visas på skärmen via indrag, 11:e nivå och
	 därefter visas med samma indrag som nivå 10
     	 alla objektobjectr kan kommenteras, förutom typ 0 och 1.
	
	 objektobjectr som stöds

	 0 - folder
	 1 - länk
	 2 - artikel
	 3 - meddelande
	 4 - e-post
	 5 - dokument
	 6 - image
	 7 - sound
	 8 - video
	 9 - publication
	 10 - task
	 

    */

{

int indent ;    /* räknare för nivå på inlägg */
int counter ;

char sCategory[32] ;        /* */ 
char sSubject[32] ;       /* */
char sRemark[255] ;              /* */

char OBJECT [128] ;

counter = 0 ; 
indent = 0 ;                   /* huvudrubriker har nivå 1 */

if ( strcmp ( EDITION, "Limited" ))
  getCategoryName ( category_id, sCategory ) ;

if (( subject_id > 0 ) && ( category_id > 0 )) 
 {
  getSubjectName ( category_id, subject_id, sSubject ) ;
  getSubjectRemark ( category_id, subject_id, sRemark ) ;
  if ( strcmp ( EDITION, "Limited" ))
    printf ("<H3>%s - %s</H3>\n", sCategory, sSubject ) ;
  else
    printf ("<H3>%s</H3>\n", sSubject ) ;
  
 /* Print remark with a smaller font size */

  if ( strlen ( sRemark ) > 0 )
   {
    printf("<FONT SIZE=-1>") ;
    printf("%s", sRemark ) ;
    printf ("</FONT>") ; 
   }
  }

if (( subject_id == 0 ) && ( category_id > 0 )) 
 {
  printf ("<H3>%s</H3>\n", sCategory ) ;

 /* Print remark with a smaller font size */

  if ( strlen ( sRemark ) > 0 )
   {
    printf("<FONT SIZE=-1>") ;
    printf("%s", sRemark ) ;
    printf ("</FONT>") ; 
   }
  }
    
printf("<p>") ;

while ( title != NULL )
 {
  if ( validUser ( user.id, title->security, SELECT, title->owner, title->group ) == TRUE ) /* skall rader skrivas ut */   
   {
   
	 if ( strlen (title->name) > 0 )
     {
	   
      if ( title->object == -1 )    /* alias */   
        strcpy ( OBJECT, "alias") ;
      if ( title->object == 0 )    /* HTML-länk */   
        strcpy ( OBJECT, "folder") ;
      if ( title->object == 1 )    /*  */ 
	   strcpy ( OBJECT, "link") ;
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
      if ( title->object == 10 )    /*  */ 
	   strcpy ( OBJECT, "item") ;
  
      printf("&#09;") ;
      for (indent=0; indent < title->indent; indent++)  /* indentering av tecken */
        printf("&#09;") ;
	
      if ( strcmp (EDITION,"Limited"))
       {
        printf("<a href=\"%s/%s%s?method=edit&category=%d&subject=%d&id=%d\"><img border=0 src=\"%s/%s.gif\"></a> <a href=\"%s/%s%s?method=select&category=%d&subject=%d&id=%d\">%s</a> ",
		      CGI_PATH, OBJECT, EXECUTABLE, title->category, title->subject, title->id, ICON_PATH, OBJECT, CGI_PATH, OBJECT, EXECUTABLE, title->category,  title->subject, title->id, title->name ) ;
	  if ( title->object > 3 ) /* skriv ut fšrfattare om inte meddelande, lŠnk, artikel */
		   if (  title->object > 8 )
		    {
			  printf("- %s, %s\n", title->author, title->email ) ;
			}
		  else
			{
          if (( strlen ( title->author ) > 0 ) && ( strlen ( title->email) > 0 ))
		    printf("- <a href=\"mailto:%s\">%s</a>\n", title->email, title->author ) ; 
		   if (( strlen ( title->author ) > 0 ))
		    printf("- %s\n", title->author ) ; 
			}
          
 
	   }  /* end of Standard Edition */
	  else
	   {
        printf("<a href=\"%s/%s%s?method=edit&subject=%d&id=%d\"><IMG SRC=\"%s/%s.gif\" border=0></a> <a href=\"%s/%s%s?method=select&subject=%d&id=%d\">%s</a> ",
			    CGI_PATH, OBJECT, EXECUTABLE, title->subject, title->id, ICON_PATH, OBJECT, CGI_PATH, OBJECT, EXECUTABLE, title->subject, title->id, title->name ) ;
		if ( title->object > 3 ) /* skriv ut fšrfattare om inte meddelande, lŠnk, artikel */
		   if (  title->object > 8 )
		    {
		    printf("- %s, %s\n", title->author, title->email ) ;
			}
		  else
			{
             if (( strlen ( title->author ) > 0 ) && ( strlen ( title->email) > 0 ))
		       printf("- <a href=\"mailto:%s\">%s</a>\n", title->email, title->author ) ; 
		     if (( strlen ( title->author ) > 0 ))
		       printf("- %s\n", title->author ) ; 
			 }

	      }   /* end of Limited  Edition */
  
      /* ny rubrik  */

	   if ( difftoday_str (  title->created  ) <=  (double) ( 60 * 60 * 24 * 7 ))
	     printf ("<IMG SRC=\"%s/new.gif\"><br>\n", ICON_PATH ) ;
	   else
	     printf("<br>\n") ;
	  
	  } /* end of str   */    
    } /* end of Vaild */
	 
   title = title->next ;
  }
	
}  /* end of printTitleHTML */
 

/*************************************************************************/

void printArticleHTML ( struct _user user, struct _article article )

{

char sLocale[255];
struct _subject subject ;

printf ("<H3>%s</H3>\n",article.title ) ;
printf("<address>") ;

  /* skriv ut vem som har skickat inlägget */

if ( strlen ( article.company ) > 0 )
  {

    if ( strlen ( article.email ) > 0 )
      printf("<a href=\"mailto:%s\">%s</a> - <strong>%s</strong> %s<br>",
        article.email, article.author, article.company, article.created ) ;
    else
      printf("%s - <strong>%s</strong> %s<br>",
        article.author, article.company, article.created ) ;
  }
else
  {
  
   if ( strlen ( article.email ) > 0 )
     printf("<a href=\"mailto:%s\">%s</a> %s<br>",
       article.email, article.author, article.created ) ;
   else
       printf("%s %s<br>", article.author, article.created ) ;
  }

printf("</address>\n") ;
printInformationClassHTML ( article.security ) ;

printf("<P>") ;

   /* är denna artikel en kommentar till en tidigare artikel */

if ( article.indent > 0 )
 {
  GetLocaleText ( MESSAGES, LOCALE, 20, sLocale ) ; /* Comment to text */
  printf("<cite>%s<a href=\"%s/article%s?method=select&", LOCALE, CGI_PATH, EXECUTABLE  ) ;
  printf("category=%d&subject=%d&id=%d\">%s</a></cite><br>\n", article.category, article.subject,  article.comment, article.reply ) ;

 }   /* end if ( article.indent > 0 ) */

  /* skriv ut inlägg om det innehåller text */

if ( article.text != NULL )
 { 
  printfHTML(  article.text ) ;
 }

/*
  för att kunna välja redigera eller ta bort gäller följande:
  om inloggad användare och ägare av artikel eller administratör
  skall man kunna redigera artikel
*/

if (((  article.owner == user.id ) && ( user.id > 0 ))|| ( user.id == 1 ))
 {
  printf("\n<table border=\"0\">") ;
  printf("<tr>\n") ;
  
  printf("<td>\n") ;
  printf("<FORM method=\"get\" action=\"%s/article%s\">", CGI_PATH, EXECUTABLE ) ;
  printf("<input type=hidden name=method value=\"edit\"> \n") ;
  printf("<input type=hidden name=category value=\"%d\">\n", article.category ) ;
  printf("<input type=hidden name=subject value=\"%d\">\n", article.subject ) ;
  printf("<input type=hidden name=id value=\"%d\">\n", article.id ) ;
  GetLocaleText ( MESSAGES, LOCALE, 13, sLocale ) ;   /* Edit */
  printf("<input type=submit value=\"%s\">", sLocale) ;
  printf ("</FORM>" ) ;
  printf("</td>\n") ;
  
  printf("<td>") ;
  printf("<FORM method=\"get\" action=\"%s/article%s\">", CGI_PATH, EXECUTABLE ) ;
  printf("<input type=hidden name=method value=\"delete\"> \n") ;
  printf("<input type=hidden name=category value=\"%d\">\n", article.category ) ;
  printf("<input type=hidden name=subject value=\"%d\">\n", article.subject ) ;
  printf("<input type=hidden name=id value=\"%d\">\n", article.id ) ;
  GetLocaleText ( MESSAGES, LOCALE, 14, sLocale ) ; /* Delete */
  printf("<input type=submit value=\"%s\">", sLocale ) ;
  printf ("</FORM>" ) ;
  printf("</td>\n") ;
   
  printf("<td>") ;
  printf("<FORM method=\"get\" action=\"%s/title%s\">", CGI_PATH, EXECUTABLE ) ;
  printf("<input type=hidden name=method value=\"move\"> \n") ;
  printf("<input type=hidden name=category value=\"%d\">\n", article.category ) ;
  printf("<input type=hidden name=subject value=\"%d\">\n", article.subject ) ;
  printf("<input type=hidden name=id value=\"%d\">\n", article.id ) ;
  
  GetLocaleText ( MESSAGES, LOCALE, 53, sLocale ) ;   /* New subject */
  
  subject.category = article.category ;
  selectSubject ( &subject,"ALL","DATE", NULL ) ; /* lds vnskade nyhetsgrupper fren fil */
  SubjectTargetListBoxHTML ( &subject ) ;
  
  GetLocaleText ( MESSAGES, LOCALE, 17, sLocale ) ;   /* Move */
  sprintf (sLocale, "Flytta") ;
  printf("<input type=submit value=\"%s\">", sLocale) ;
  printf ("</FORM>" ) ;
  printf("</td>\n") ;
  
  printf ("</tr>\n") ; 
  printf("</table>\n") ;

 }   /* end of if */


}  /* end of printArticleHTML */

void printPublicationHTML ( struct _user user, struct _publication publication )

{

char sLocale[255];
struct _subject subject ;
printf ("<H3>%s</H3>\n", publication.title ) ;

  /* skriv ut författare/artist och eventuellt  förlag */

 if ( publication.media < LP )
     printf("Författare: ") ;
  else
    printf("Artist/grupp: ") ;

if ( strlen ( publication.company ) > 0 )
  {
  
     if ( strlen ( publication.email ) > 0 )
      printf(" %s <br>F&ouml;rlag: <strong>%s</strong><br>",
        publication.author, publication.email ) ;
    else
      printf("%s <br>",
        publication.author ) ;
  }
else
  {
  
   if ( strlen ( publication.email ) > 0 )
     printf("%s <br>F&ouml;rlag: <strong>%s</strong> <br>",
       publication.author, publication.email ) ;
   else
       printf("%s<br>", publication.author ) ;
  }

printf ("Media: ") ;
if ( publication.media  == BOOK )
 printf("Bok \n<br>\n") ;

if ( publication.media == MAGAZINE )
 printf("Tidskrift \n<br>\n") ;

if ( publication.media == NEWSPAPER )
 printf("Tidning\n<br>\n") ;

if ( publication.media == LP  )
 printf("Vinyl\n<br>\n") ;

if ( publication.media == AUDIO_TAPE  )
 printf("Kassettband\n<br>\n") ;

if ( publication.media == VIDEO_TAPE )
 printf("Videoband\n<br>\n") ;

if ( publication.media == CD )
 printf("CD \n<br>\n") ;

if ( publication.media == DAT )
 printf("DAT \n<br>\n") ;

if ( publication.media == DVD )
 printf("DVD \n<br>\n") ;

if ( publication.media == 99 )
 printf("&Ouml;vrigt \n<br>\n") ;

  /* skriv ut text om det finns någon */

printInformationClassHTML ( publication.security ) ;

printf("<P>\n") ;

if ( publication.text != NULL )
 {   
  printfHTML(  publication.text ) ;
 }

/*
  för att kunna välja redigera eller ta bort gäller följande:
  om inloggad användare och ägare av artikel eller administratör
  skall man kunna redigera artikel
*/

if ((( publication.owner == user.id ) && ( user.id > 0 )) || ( user.id == 1 ))
 {
  printf("\n<table border=\"0\"> <tr><td>\n") ;
  printf("<FORM method=\"get\" action=\"%s/publication%s\">", CGI_PATH, EXECUTABLE ) ;
  printf("<input type=hidden name=method value=\"edit\"> \n") ;
  printf("<input type=hidden name=category value=\"%d\">\n", publication.category ) ;
  printf("<input type=hidden name=subject value=\"%d\">\n", publication.subject ) ;
  printf("<input type=hidden name=id value=\"%d\">\n", publication.id ) ;
  GetLocaleText ( MESSAGES, LOCALE, 13, sLocale ) ;   /* Edit */
  printf("<input type=submit value=\"%s\">", sLocale) ;
  printf ("</FORM>" ) ;
  printf("</td><td>") ;

  printf("<FORM method=\"get\" action=\"%s/article%s\">", CGI_PATH, EXECUTABLE ) ;
  printf("<input type=hidden name=method value=\"delete\"> \n") ;
  printf("<input type=hidden name=category value=\"%d\">\n", publication.category ) ;
  printf("<input type=hidden name=subject value=\"%d\">\n", publication.subject ) ;
  printf("<input type=hidden name=id value=\"%d\">\n", publication.id ) ;
  GetLocaleText ( MESSAGES, LOCALE, 14, sLocale ) ; /* Delete */
  printf("<input type=submit value=\"%s\">", sLocale ) ;
  printf ("</FORM>" ) ;

  printf ("</td>") ;
   printf("<td>") ;
  printf("<FORM method=\"get\" action=\"%s/title%s\">", CGI_PATH, EXECUTABLE ) ;
  printf("<input type=hidden name=method value=\"move\"> \n") ;
  printf("<input type=hidden name=category value=\"%d\">\n", publication.category ) ;
  printf("<input type=hidden name=subject value=\"%d\">\n", publication.subject ) ;
  printf("<input type=hidden name=id value=\"%d\">\n", publication.id ) ;
  printf("<input type=hidden name=email value=\"%d\">\n", publication.category ) ;
  
  GetLocaleText ( MESSAGES, LOCALE, 53, sLocale ) ;   /* New subject */
  
  subject.category = publication.category ;
  selectSubject ( &subject,"ALL","DATE", NULL ) ; /* lds vnskade nyhetsgrupper fren fil */
  SubjectTargetListBoxHTML ( &subject ) ;
  
  GetLocaleText ( MESSAGES, LOCALE, 17, sLocale ) ;   /* Move */
  sprintf (sLocale, "Flytta") ;
  printf("<input type=submit value=\"%s\">", sLocale) ;
  printf ("</FORM>" ) ;
  printf("</td>\n") ;
  printf("</tr></table>") ;
 }   /* end of if */


}  /* end of printPublicationHTML */

void printMessageHTML ( struct _user user, struct _message message )

{

char sLocale[255] ;
struct _subject subject ;

printf ("<H3>%s</H3>\n",message.title ) ;

   /* är denna artikel en kommentar till en tidigare artikel */

if ( message.indent > 0 )
 {
 
  GetLocaleText ( MESSAGES, LOCALE, 20, sLocale ) ; /* Comment to text */
  printf("<cite>%s<a href=\"%s/article%s?method=select&", sLocale, CGI_PATH, EXECUTABLE  ) ;
  printf("category=%d&subject=%d&id=%d\">%s</a></cite><br>", message.category, message.subject,  message.comment, message.reply ) ;

 }   /* end if ( message.indent > 0 ) */

  /* skriv ut inlägg om det innehåller text */

if ( message.text != NULL )
 { 
  printf("%s\n", message.text ) ;
 }

 printf("<address>") ;

  /* skriv ut vem som har skickat inlägget */

   /* skriv ut vem som har skickat inlägget */

if ( strlen ( message.company ) > 0 )
  {
	  GetLocaleText ( MESSAGES, LOCALE, 9, sLocale ) ; /*Author */
    if ( strlen ( message.email ) > 0 )
      printf("%s: <a href=\"mailto:%s\">%s</a> - <strong>%s</strong><br>",
        sLocale, message.email, message.author, message.company ) ;
    else
      printf("%s: %s - <strong>%s</strong><br>",
        sLocale, message.author, message.company ) ;
  }
else
  {
	  GetLocaleText ( MESSAGES, LOCALE, 9, sLocale ) ; /* Author */
   if ( strlen ( message.email ) > 0 )
     printf("%s: <a href=\"mailto:%s\">%s</a><br>",
        sLocale, message.email, message.author ) ;
   else
       printf("%s: %s<br>", sLocale, message.author ) ;
  }


GetLocaleText ( MESSAGES, LOCALE, 22, sLocale ) ;   /* Date */

printf("%s: %s<br></address>\n", sLocale, message.created ) ;
printInformationClassHTML ( message.security ) ;
printf("<P>") ;


printf("<table><tr><td>\n") ;
printf("<FORM method=\"post\" action=\"%s/comment%s\">", CGI_PATH, EXECUTABLE ) ;
printf("<input type=hidden name=method value=\"edit\"> \n") ;
printf("<input type=hidden name=category value=\"%d\">\n", message.category ) ;
printf("<input type=hidden name=subject value=\"%d\">\n", message.subject ) ;
printf("<input type=hidden name=id value=\"%d\">\n", message.id ) ;
printf("<input type=submit value=\"Kommentera \">") ;

printf ("</FORM>\n" ) ;
printf("</td>\n") ;
/*
  för att kunna välja redigera eller ta bort gäller följande:
  om inloggad användare och ägare av artikel eller administratör
  skall man kunna redigera artikel
*/

if ((( message.owner == user.id ) && ( user.id > 0 ))|| ( user.id == 1 ))
 {
  printf("<td>\n") ;
  printf("<FORM method=\"get\" action=\"%s/message%s\">", CGI_PATH, EXECUTABLE ) ;
  printf("<input type=hidden name=method value=\"edit\"> \n") ;
  printf("<input type=hidden name=category value=\"%d\">\n", message.category ) ;
  printf("<input type=hidden name=subject value=\"%d\">\n", message.subject ) ;
  printf("<input type=hidden name=id value=\"%d\">\n", message.id ) ;
  printf("<input type=submit value=\"Redigera \">") ;

  printf ("</FORM>" ) ;
  printf("</td><td>") ;
  printf("<FORM method=\"get\" action=\"%s/article%s\">", CGI_PATH, EXECUTABLE ) ;
  printf("<input type=hidden name=method value=\"delete\"> \n") ;
  printf("<input type=hidden name=category value=\"%d\">\n", message.category ) ;
  printf("<input type=hidden name=subject value=\"%d\">\n", message.subject ) ;
  printf("<input type=hidden name=id value=\"%d\">\n", message.id ) ;
  printf("<input type=submit value=\"Ta bort \">") ;

  printf ("</FORM>" ) ;
  printf("</td>") ;

  printf("<td>") ;
  printf("<FORM method=\"get\" action=\"%s/title%s\">", CGI_PATH, EXECUTABLE ) ;
  printf("<input type=hidden name=method value=\"move\"> \n") ;
  printf("<input type=hidden name=category value=\"%d\">\n", message.category ) ;
  printf("<input type=hidden name=subject value=\"%d\">\n", message.subject ) ;
  printf("<input type=hidden name=id value=\"%d\">\n", message.id ) ;
   
  GetLocaleText ( MESSAGES, LOCALE, 53, sLocale ) ;   /* New subject */
  
  subject.category = message.category ;
  selectSubject ( &subject,"ALL","DATE", NULL ) ; /* lds vnskade nyhetsgrupper fren fil */
  SubjectTargetListBoxHTML ( &subject ) ;
  
  GetLocaleText ( MESSAGES, LOCALE, 17, sLocale ) ;   /* Move */
  sprintf (sLocale, "Flytta") ;
  printf("<input type=submit value=\"%s\">", sLocale) ;
  printf ("</FORM>" ) ;
  printf("</td>\n") ;

 }   /* end of if */
printf ("</tr></table>") ;

}  /* end of printMessageHTML */


void printLinkHTML ( struct _link link )

{

printf("<HTML>") ;
printf("<META HTTP-EQUIV=\"Refresh\" CONTENT=\"1;URL=%s\">", link.href) ;
printf("</HTML>") ;
}

/**********************************************************/

void SubjectListBoxHTML ( struct _subject *subject )


{

printf("<SELECT name=\"subject\">") ;

while ( subject != NULL )
 {
  printf ("<OPTION value=\"%d\">%s\n", subject->id, subject->name ) ;
  subject = subject->next ;
 }
printf("</SELECT>") ;

}

void SubjectTargetListBoxHTML ( struct _subject *subject )

{

printf("<SELECT name=\"to\">") ;

while ( subject != NULL )
 {
  printf ("<OPTION value=\"%d\">%s\n", subject->id, subject->name ) ;
  subject = subject->next ;
 }
printf("</SELECT>") ;

}

void  InformationClassListBoxHTML ( int iInformationClass ) 

{

printf("<SELECT name=\"public\">") ;

if (iInformationClass  == 0 ) printf("<OPTION value=\"%d\">External\n", iInformationClass ) ;
if (iInformationClass  == 1 ) printf("<OPTION value=\"%d\">Public\n", iInformationClass ) ;
if (iInformationClass  == 2 ) printf("<OPTION value=\"%d\">Internal\n", iInformationClass ) ;
if (iInformationClass  == 3 ) printf("<OPTION value=\"%d\">Confidential\n", iInformationClass ) ;
if (iInformationClass  == 4 ) printf("<OPTION value=\"%d\">Secret\n", iInformationClass ) ;
if (iInformationClass  == 5 ) printf("<OPTION value=\"%d\">Private\n", iInformationClass ) ;
if (iInformationClass  == 6 ) printf("<OPTION value=\"%d\">Personal\n", iInformationClass ) ;

printf("<OPTION value=\"-1\">--------\n") ;

if (iInformationClass  != 0 ) printf("<OPTION value=\"0\">External\n") ;
if (iInformationClass  != 1 ) printf("<OPTION value=\"1\">Public\n") ;
if (iInformationClass  != 2 ) printf("<OPTION value=\"2\">Internal\n") ;
if (iInformationClass  != 3 ) printf("<OPTION value=\"3\">Confidential\n") ;
if (iInformationClass  != 4 ) printf("<OPTION value=\"4\">Secret\n") ;
if (iInformationClass  != 5 ) printf("<OPTION value=\"5\">Private\n") ;
if (iInformationClass  != 6 ) printf("<OPTION value=\"6\">Personal\n") ;

printf("</SELECT>") ;

}

void  MediaListBoxHTML ( ) 

{

printf("<SELECT name=\"media\">") ;

printf("<OPTION value=\"0\">\n") ;
printf("<OPTION value=\"1\">Bok\n") ;
printf("<OPTION value=\"2\">Tidskrift\n") ;
printf("<OPTION value=\"3\">Tidning\n") ;
printf("<OPTION value=\"4\">LP\n") ;
printf("<OPTION value=\"5\">Kassettband\n") ;
printf("<OPTION value=\"6\">Videoband\n") ;
printf("<OPTION value=\"7\">DAT\n") ;
printf("<OPTION value=\"8\">CD\n") ;
printf("<OPTION value=\"9\">DVD\n") ;
printf("<OPTION value=\"99\">&Ouml;vrigt\n") ;

printf("</SELECT>") ;

}

void CategoryListBoxHTML ( struct _category *category )

{

printf("<SELECT name=\"category\">") ;

while ( category != NULL )
 {
  printf ("<OPTION value=\"%d\">%s\n", category->id, category->name ) ;
  category = category->next ;
 }
printf("</SELECT>") ;

}

void TextBoxHTML ( char *name, char *value, int size )

{

printf("<input name=\"%s\" value=\"%s\" size=%d>", name, value, size) ;

}

void EditBoxHTML ( char *name, char *value, int rows, int cols )

{

printf("<textarea name=\"%s\" rows=\"%d\" cols=%d>", name, rows, cols) ;
printf("%s</textarea>", value ) ;

}

/**/

void printMessageForm ( struct _message message )

{

char sCategory[32] ;
char sSubject[32] ;
char sLocale[255] ;

struct _subject subject ;

if ( strcmp ( EDITION, "Limited" ))
  getCategoryName ( message.category, sCategory ) ;

if ( message.subject > 0 )
  getSubjectName ( message.category, message.subject, sSubject ) ;

if ( message.id != 0 )
  GetLocaleText ( MESSAGES, LOCALE, 25, sLocale ) ;
else
  GetLocaleText ( MESSAGES, LOCALE, 24, sLocale ) ;

printf("<H2>%s</H2>\n", sLocale ) ;

printf("<FORM method=\"post\" action=\"%s/message%s\">\n", CGI_PATH, EXECUTABLE ) ;

if ( message.id  != 0 )   /**/
  {
   printf( "<input type=hidden name=method value=\"update\"> \n") ;
   printf(" <input type=hidden name=id value=\"%d\">\n", message.id ) ;
 }
else
  {
  printf("<input type=hidden name=method value=\"insert\"> \n") ;
  
  }

if ( strcmp ( EDITION, "Limited"))
  printf( "<input type=hidden name=category  value=\"%d\"> \n", message.category ) ;

printf("<table border=0>\n") ;

if ( strcmp ( EDITION, "Limited"))
 {
  GetLocaleText ( MESSAGES, LOCALE, 3, sLocale ) ; /* Category */

  printf("<tr>") ;
  printf("<td>%s:</td>", sLocale) ;
  printf("<td> %s", sCategory ) ;

  printf("</td>") ;
  printf("</tr>\n") ;
 }

GetLocaleText ( MESSAGES, LOCALE, 4, sLocale ) ; /* Subject */
printf("<tr>") ;
printf("<td>%s:</td>", sLocale) ;
printf("<td>") ;

subject.category = message.category ;

if ( message.subject == 0 )
 {
   selectSubject ( &subject,"ALL","DATE", NULL ) ; /* läs önskade nyhetsgrupper från fil */
  SubjectListBoxHTML ( &subject ) ;
  }
else
  {
   printf("%s", sSubject ) ;
   printf("<input type=hidden name=subject value=\"%d\">", message.subject ) ;
  }

printf("</td></td>\n") ;

GetLocaleText ( MESSAGES, LOCALE, 6, sLocale ) ;

printf("<tr><td>%s: </td><td>", sLocale) ;
TextBoxHTML ( "title", message.title, 25 ) ;
printf("</td></tr>") ;

printf("<tr><td>%s:</td><td>", "Information Class") ;
InformationClassListBoxHTML ( message.security ) ;
printf("</td></tr>") ;

GetLocaleText ( MESSAGES, LOCALE, 7, sLocale ) ;

printf("<tr><td valign=\"top\">%s:<td>", sLocale) ;
EditBoxHTML ( "text", message.text, 10, 50 ) ;
printf("</td></tr>") ;

 /* om annonym användare så skall programmet fråga efter namn och e-postadress */

if ( strlen ( message.author ) > 0  )
  {
  GetLocaleText ( MESSAGES, LOCALE, 9, sLocale ) ; /* author */
  printf("<tr><td>%s</td><td>%s", sLocale, message.author ) ;
  printf("</td></tr>") ;
  printf ("<input type=hidden name=author value=\"%s\">", message.author ) ;
  printf ("<input type=hidden name=email value=\"%s\">", message.email ) ;
  }
else
 {
  GetLocaleText ( MESSAGES, LOCALE, 9, sLocale ) ; /* author */
  printf("<tr><td>%s:<td>",sLocale ) ;
  TextBoxHTML ( "author", message.author, 25 ) ;
  printf("</td></tr>") ;

  GetLocaleText ( MESSAGES, LOCALE, 8, sLocale ) ; /* e-mail */
  printf("<tr><td valign=\"top\">%s:<td>", sLocale) ;
  TextBoxHTML ( "email", message.email, 25 ) ;
  printf("</td></tr>") ;
 }

printf("<tr><td></td><td>" ) ;
GetLocaleText ( MESSAGES, LOCALE, 10, sLocale ) ;
printf("<input type=submit value=\"%s\">", sLocale) ;
GetLocaleText ( MESSAGES, LOCALE, 11, sLocale ) ;
printf("<input type=reset value=\"%s\">", sLocale ) ;
printf ("</td></tr></table>") ;
printf ("</FORM>" ) ;

}

void printLinkForm ( struct _link link )

{

char sCategory[32] ;
char sSubject[32] ;
char sLocale[255] ;

struct _subject subject ;

if ( strcmp ( EDITION, "Limited" ))
  getCategoryName ( link.category, sCategory ) ;

if ( link.subject > 0 )
  getSubjectName ( link.category, link.subject, sSubject ) ;

if ( link.id != 0 )
  GetLocaleText ( MESSAGES, LOCALE, 32, sLocale ) ;  /* Edit link */
else
  GetLocaleText ( MESSAGES, LOCALE, 31, sLocale ) ;  /* New link */

printf("<H2>%s</H2>\n", sLocale ) ;     

printf("<FORM method=\"post\" action=\"%s/link%s\">\n", CGI_PATH, EXECUTABLE ) ;

if ( link.id  != 0 )
{
   printf( "<input type=hidden name=method value=\"update\"> \n") ;
   printf ("<input type=hidden name=group value=\"%d\">", link.group ) ;
}
else
{  printf("<input type=hidden name=method value=\"insert\"> \n") ;
   printf ("<input type=hidden name=group value=\"%d\">", -1 ) ;
}

if ( strcmp ( EDITION, "Limited"))
  printf( "<input type=hidden name=category  value=\"%d\"> \n", link.category ) ;

printf("<table border=0>\n") ;

if ( strcmp ( EDITION, "Limited"))
 {

  GetLocaleText ( MESSAGES, LOCALE, 3, sLocale ) ;  /* category */

  printf("<tr>") ;
  printf("<td>%s:</td>", sLocale) ;
  printf("<td> %s", sCategory ) ;

  printf("</td>") ;
  printf("</tr>\n") ;
 }

GetLocaleText ( MESSAGES, LOCALE, 4, sLocale ) ;  /* subject */

printf("<tr>") ;
printf("<td>%s:</td>", sLocale) ;
printf("<td>") ;

subject.category = link.category ;

if ( link.subject == 0 )
 {
  selectSubject ( &subject,"ALL","DATE", NULL ) ; /* läs önskade nyhetsgrupper från fil */
  SubjectListBoxHTML ( &subject ) ;
  }
else
  {
   printf("%s", sSubject ) ;
   printf("<input type=hidden name=subject value=\"%d\">", link.subject ) ;
  }

printf("</td></td>\n") ;

GetLocaleText ( MESSAGES, LOCALE, 6, sLocale ) ;  /* title */

printf("<tr><td>%s:</td><td>", sLocale) ;
TextBoxHTML ( "title", link.title, 25 ) ;
printf("</td></tr>") ;

strcpy ( link.href, "http://") ;
GetLocaleText ( MESSAGES, LOCALE, 30, sLocale ) ;  /* Link */

printf("<tr><td valign=\"top\">%s:<td>", sLocale ) ;
TextBoxHTML ( "link", link.href, 64 ) ;
printf("</td></tr>") ;

 /* om annonym användare så skall programmet fråga efter namn och e-postadress */

if ( strlen ( link.author ) > 0  )
  {
  GetLocaleText ( MESSAGES, LOCALE, 9, sLocale ) ; /* Name */

  printf("<tr><td>%s:</td><td>%s", sLocale, link.author ) ;
  printf("</td></tr>") ;
  printf ("<input type=hidden name=author value=\"%s\">", link.author ) ;
  printf ("<input type=hidden name=email value=\"%s\">", link.email ) ;
  }
else
 {
  GetLocaleText ( MESSAGES, LOCALE, 9, sLocale ) ;  /* Name */

  printf("<tr><td>%s:<td>", sLocale ) ;
  TextBoxHTML ( "author", link.author, 25 ) ;
  printf("</td></tr>") ;

  GetLocaleText ( MESSAGES, LOCALE, 8, sLocale ) ;   /* E-mail */

  printf("<tr><td valign=\"top\">%s:<td>", sLocale) ;
  TextBoxHTML ( "email", link.email, 25 ) ;
  printf("</td></tr>") ;
 }

printf("</table>") ;
printf("<table>") ;

printf("<tr><td></td><td>" ) ;

GetLocaleText ( MESSAGES, LOCALE, 10, sLocale ) ;    /* Save */
printf("<input type=submit value=\"%s \">", sLocale) ;

GetLocaleText ( MESSAGES, LOCALE, 11, sLocale ) ;    /* Cancel */
printf("<input type=reset value=\"%s\">", sLocale ) ;
printf ("</td></tr></table>") ;
printf ("</FORM>" ) ;

}


/*****************************************************************/

void printArticleForm ( struct _article article )

{
char sCategory[32] ;
char sSubject[32] ;
char sInformationClass[32] ;
char sLocale[255] ;

struct _subject subject ;

if ( strcmp ( EDITION, "Limited" ))
  getCategoryName ( article.category, sCategory ) ;

if ( article.subject > 0 )
  getSubjectName ( article.category, article.subject, sSubject ) ;

if ( article.id != 0 )
  GetLocaleText ( MESSAGES, LOCALE, 42, sLocale ) ; /* Edit article*/
else
  GetLocaleText ( MESSAGES, LOCALE, 41, sLocale ) ; /* New article */

printf("<H2>%s</H2>\n", sLocale ) ;

printf("<FORM method=\"post\" action=\"%s/article%s\">\n", CGI_PATH, EXECUTABLE ) ;

if ( article.id  != 0 )
  {
   printf( "<input type=hidden name=method value=\"update\"> \n") ;
   printf(" <input type=hidden name=id value=\"%d\">\n", article.id ) ;
   printf(" <input type=hidden name=group value=\"%d\">\n", article.group ) ;  }
else
  {
  printf("<input type=hidden name=method value=\"insert\"> \n") ;
  printf(" <input type=hidden name=group value=\"%d\">\n", -1 ) ;
  }


if ( strcmp ( EDITION, "Limited"))
  printf( "<input type=hidden name=category  value=\"%d\"> \n", article.category ) ;


printf("<table border=0>\n") ;

if ( strcmp ( EDITION, "Limited"))
 {

  GetLocaleText ( MESSAGES, LOCALE, 3, sLocale ) ; /* Category */
  printf("<tr>") ;
  printf("<td>%s:</td>", sLocale) ;
  printf("<td> %s", sCategory ) ;

  printf("</td>") ;
  printf("</tr>\n") ;
 }

printf("<tr>") ;

GetLocaleText ( MESSAGES, LOCALE, 4, sLocale ) ; /* subject */
printf("<td>%s:</td>", sLocale) ;
printf("<td>" ) ;

subject.category = article.category ;

if ( article.subject == 0 )
  {
    selectSubject ( &subject,"ALL","DATE", NULL ) ; /* lds vnskade nyhetsgrupper fren fil */
    SubjectListBoxHTML ( &subject ) ;
  }
else
  {
   printf("%s", sSubject ) ;
   printf("<input type=hidden name=subject value=\"%d\">", article.subject ) ;
  }

printf("</td></td></tr>\n") ;

GetLocaleText ( MESSAGES, LOCALE, 6, sLocale ) ; /* Title */
printf("<tr><td>%s:</td><td>", sLocale) ;
TextBoxHTML ( "title", article.title, 25 ) ;
printf("</td></tr>\n") ;

printf("<tr><td>%s:</td><td>", "Information Class") ;
InformationClassListBoxHTML ( article.security ) ;
printf("</td></tr>") ;

GetLocaleText ( MESSAGES, LOCALE, 7, sLocale ) ; /* Text */

printf("<tr><td valign=\"top\">%s: <td>", sLocale ) ;
EditBoxHTML ( "text", article.text, 10, 50 ) ;
printf("</td></tr>\n") ;
  
 /* om annonym användare så skall programmet fråga efter namn och e-postadress */

if ( strlen ( article.author ) > 0  )
  {

  GetLocaleText ( MESSAGES, LOCALE, 9, sLocale ) ; /* Name */
  printf("<tr><td>%s: </td><td>%s", sLocale, article.author ) ;
  printf("</td></tr>\n") ;
  printf ("<input type=hidden name=author value=\"%s\">", article.author ) ;
  printf ("<input type=hidden name=email value=\"%s\">", article.email ) ;
  }
else
 {
  GetLocaleText ( MESSAGES, LOCALE, 9, sLocale ) ; /* Name */
  printf("<tr><td>%s: </td><td>", sLocale) ;
  TextBoxHTML ( "author", article.author, 25 ) ;
  printf("</td></tr>\n") ;
  GetLocaleText ( MESSAGES, LOCALE, 8, sLocale ) ; /* E-mail */

  printf("<tr><td valign=\"top\">%s: <td>", sLocale ) ;
  TextBoxHTML ( "email", article.email, 25 ) ;
  printf("</td></tr>\n") ;
 }
printf("</table>\n") ;
printf("<table>") ;
printf("<tr><td>" ) ;
GetLocaleText ( MESSAGES, LOCALE, 10, sLocale ) ; /* Save */
printf("<input type=submit value=\"%s \">", sLocale ) ;
printf("</td><td>") ;
GetLocaleText ( MESSAGES, LOCALE, 11, sLocale ) ; /* Cancel */
printf("<input type=reset value=\"%s\">", sLocale ) ;

printf ("</td></tr></table>\n") ;
printf ("</FORM>" ) ;

}

void printPublicationForm ( struct _publication publication )

{
char sCategory[32] ;
char sSubject[32] ;
char sLocale[255] ;

struct _subject subject ;

if ( strcmp ( EDITION, "Limited" ))
  getCategoryName ( publication.category, sCategory ) ;

if ( publication.subject > 0 )
  getSubjectName ( publication.category, publication.subject, sSubject ) ;

if (publication.id != 0 )
  GetLocaleText ( MESSAGES, LOCALE, 82, sLocale ) ; /* Edit publication*/
else
  GetLocaleText ( MESSAGES, LOCALE, 81, sLocale ) ; /* New publication */

printf("<H2>%s</H2>\n", sLocale ) ;

printf("<FORM method=\"post\" action=\"%s/publication%s\">\n", CGI_PATH, EXECUTABLE ) ;

if ( publication.id  != 0 )
  {
   printf( "<input type=hidden name=method value=\"update\"> \n") ;
   printf(" <input type=hidden name=id value=\"%d\">\n", publication.id ) ;
   printf ("<input type=hidden name=group value=\"%d\">", publication.group ) ;

  }
else
  {
  printf("<input type=hidden name=method value=\"insert\"> \n") ;
  printf ("<input type=hidden name=group value=\"%d\">", publication.group ) ;
  }


if ( strcmp ( EDITION, "Limited"))
  printf( "<input type=hidden name=category  value=\"%d\"> \n", publication.category ) ;


printf("<table border=0>\n") ;

if ( strcmp ( EDITION, "Limited"))
 {

  GetLocaleText ( MESSAGES, LOCALE, 3, sLocale ) ; /* Category */
  printf("<tr>") ;
  printf("<td>%s:</td>", sLocale) ;
  printf("<td> %s", sCategory ) ;

  printf("</td>") ;
  printf("</tr>\n") ;
 }

printf("<tr>") ;

GetLocaleText ( MESSAGES, LOCALE, 4, sLocale ) ; /* subject */
printf("<td>%s:</td>", sLocale) ;
printf("<td>" ) ;

subject.category = publication.category ;

if ( publication.subject == 0 )
  {
    selectSubject ( &subject,"ALL","DATE", NULL ) ; /* lds ŠmnesomrŒden nyhetsgrupper fren fil */
    SubjectListBoxHTML ( &subject ) ;
  }
else
  {
   printf("%s", sSubject ) ;
   printf("<input type=hidden name=subject value=\"%d\">", publication.subject ) ;
  }

printf("</td></td></tr>\n") ;


GetLocaleText ( MESSAGES, LOCALE, 6, sLocale ) ; /* Title */
printf("<tr><td>%s:</td><td>", sLocale) ;
TextBoxHTML ( "title", publication.title, 25 ) ;
printf("</td></tr>\n") ;

printf("<tr><td>%s:</td><td>", "Information Class") ;
InformationClassListBoxHTML ( publication.security ) ;
printf("</td></tr>") ;

printf("<tr><td>%s:</td><td>", "Media") ;
MediaListBoxHTML () ;
printf("</td></tr>") ;

GetLocaleText ( MESSAGES, LOCALE, 7, sLocale ) ; /* Text */

printf("<tr><td valign=\"top\">%s: <td>", sLocale ) ;
EditBoxHTML ( "text", publication.text, 10, 50 ) ;
printf("</td></tr>\n") ;

GetLocaleText ( MESSAGES, LOCALE, 21, sLocale ) ; /* Fšrfattare*/
printf("<tr><td>%s: </td><td>", sLocale) ;
TextBoxHTML ( "author", publication.author, 25 ) ;
printf("</td></tr>\n") ;

GetLocaleText ( MESSAGES, LOCALE, 83, sLocale ) ; /* Fšrlag */
printf("<tr><td valign=\"top\">%s: <td>", sLocale ) ;
TextBoxHTML ( "email", publication.email, 25 ) ;
printf("</td></tr>\n") ;

printf("</table>\n") ;
printf("<table>") ;
printf("<tr><td>" ) ;
GetLocaleText ( MESSAGES, LOCALE, 10, sLocale ) ; /* Save */
printf("<input type=submit value=\"%s \">", sLocale ) ;
printf("</td><td>") ;
GetLocaleText ( MESSAGES, LOCALE, 11, sLocale ) ; /* Cancel */
printf("<input type=reset value=\"%s\">", sLocale ) ;

printf ("</td></tr></table>\n") ;
printf ("</FORM>" ) ;

}


/*
   HTML form for making att komment to an object
   Can be used for every object object exept 0 and 1.

*/

void printCommentForm ( struct _message message )

{

char sCategory[32] ;
char sSubject[32] ;
char sLocale[255];

struct _subject subject ;

if ( strcmp ( EDITION, "Limited" ))
  getCategoryName ( message.category, sCategory ) ;

if ( message.subject > 0 )
  getSubjectName ( message.category, message.subject, sSubject ) ;

GetLocaleText ( MESSAGES, LOCALE, 27, sLocale ) ; /* New comment */

printf("<H2>%s</H2>", sLocale ) ;

printf("<FORM method=\"post\" action=\"%s/message%s\">", CGI_PATH, EXECUTABLE ) ;

printf("<input type=hidden name=method value=\"insert\"> \n") ;

if ( message.category > 0 )
  printf( "<input type=hidden name=category  value=\"%d\"> \n", message.category ) ;

printf("<input type=hidden name=group value=\"%d\"> \n", message.group ) ;
printf("<input type=hidden name=subject value=\"%d\"> \n", message.subject ) ;
printf("<input type=hidden name=comment value=\"%d\" >\n", message.comment );
printf("<input type=hidden name=indent value=\"%d\" >\n", message.indent );
printf("<table border=0>\n") ;

if ( message.category > 0 )
{
  GetLocaleText ( MESSAGES, LOCALE, 3, sLocale ) ; /* Category */

  printf("<tr>") ;
  printf("<td>%s: </td>", sLocale) ;
  printf("<td>%s", sCategory ) ;

  printf("</td>") ;
  printf("</tr>\n<tr>") ;
}

GetLocaleText ( MESSAGES, LOCALE, 4, sLocale ) ; /* subject */
printf("<td>%s: </td>", sLocale ) ;
printf("<td>") ;
subject.category = message.category ;
if ( message.subject == 0 )
  {
    selectSubject ( &subject,"ALL","DATE", NULL ) ; /* lds vnskade nyhetsgrupper fren fil */
    SubjectListBoxHTML ( &subject ) ;
  }
else
  {
   printf("%s", sSubject ) ;
   printf("<input type=hidden name=subject value=\"%d\">", message.subject ) ;
  }

printf("</td></td></tr>\n") ;

GetLocaleText ( MESSAGES, LOCALE, 6, sLocale ) ; /* Title */
printf("<tr><td>%s: </td><td valign=\"top\">", sLocale ) ;

TextBoxHTML ("title",message.title, 25 ) ;
printf("</td></tr>") ;

printf("<tr><td>%s:</td><td>", "Information Class") ;
InformationClassListBoxHTML ( message.security ) ;
printf("</td></tr>") ;

GetLocaleText ( MESSAGES, LOCALE, 7, sLocale ) ; /* Text */
printf("<tr><td valign=\"top\">%s: </td><td valign=\"top\">", sLocale ) ;
EditBoxHTML ( "text", "", 10,50 ) ;
printf("</td></tr>") ;

 /* om annonym användare så skall programmet fråga efter namn och e-postadress */

if ( strlen ( message.author ) > 0  )
  {

  GetLocaleText ( MESSAGES, LOCALE, 9, sLocale ) ; /* Name */

  printf("<tr><td>%s: </td><td>%s", sLocale, message.author ) ;
  printf("</td></tr>") ;
  printf ("<input type=hidden name=author value=\"%s\">", message.author ) ;
  printf ("<input type=hidden name=email value=\"%s\">", message.email ) ;
  }
else
 {

  GetLocaleText ( MESSAGES, LOCALE, 9, sLocale ) ; /* Name */
  printf("<tr><td>%s: <td>", sLocale ) ;
  TextBoxHTML ( "author", message.author, 25 ) ;
  printf("</td></tr>") ;

  GetLocaleText ( MESSAGES, LOCALE, 8, sLocale ) ; /* E-mail */
  printf("<tr><td valign=\"top\">%s: <td>", sLocale ) ;
  TextBoxHTML ( "email", message.email, 25 ) ;
  printf("</td></tr>") ;
 }

printf("</table>") ;
printf("<table>") ;

printf("<tr><td></td><td>" ) ;
GetLocaleText ( MESSAGES, LOCALE, 10, sLocale ) ; /* Save */
printf("<input type=submit value=\"%s \">", sLocale ) ;
GetLocaleText ( MESSAGES, LOCALE, 11, sLocale ) ; /* Cancel */
printf("<input type=reset value=\"%s \">", sLocale ) ;
printf ("</td></tr></table>") ;
printf ("</FORM>" ) ;


}

/*******************************************************************/

void printSubjectFormHTML ( struct _subject subject )

{

char sLocale[255] ;


GetLocaleText ( MESSAGES, LOCALE, 53, sLocale ) ; /* New subject */

printf ("<H2>%s</H2>", sLocale ) ;

printf("<FORM method=\"post\" action=\"%s/subject%s\">", CGI_PATH, EXECUTABLE ) ;

printf("<input type=hidden name=method value=\"insert\"> \n") ;

printf("<table border=0>\n") ;

 if ( strcmp( EDITION, "Limited"))      /* Om Edition Limited skall  inte denna raden skrivas ut */
  printf( "<input type=hidden name=category  value=\"%d\"> \n", subject.category ) ;


GetLocaleText ( MESSAGES, LOCALE, 4, sLocale ) ; /* Subject */

printf("<tr><td>%s: </td><td valign=\"top\">", sLocale ) ;
TextBoxHTML ( "name",subject.name, 25 ) ;
printf("</td></tr>") ;

GetLocaleText ( MESSAGES, LOCALE, 7, sLocale ) ; /* Text */
printf("<tr><td>%s:<td valign=\"top\">", sLocale ) ;
EditBoxHTML ( "remark", "", 5,35 ) ;
printf("</td></tr>") ;

printf("<tr><td></td><td>" ) ;
GetLocaleText ( MESSAGES, LOCALE, 10, sLocale ) ; /* Save */
printf("<input type=submit value=\"%s \">", sLocale ) ;
GetLocaleText ( MESSAGES, LOCALE, 11, sLocale ) ; /* Cancel */
printf("<input type=reset value=\"%s\">", sLocale ) ;
printf ("</td></tr></table>") ;
printf ("</FORM>" ) ;

}

/*

 HTML form used to create a new category

*/

void printCategoryFormHTML ( struct _category category )

{

char sLocale[255] ;

GetLocaleText ( MESSAGES, LOCALE, 52, sLocale ) ; /* New category */

printf ("<H2>%s</H2>", sLocale ) ;

printf("<FORM method=\"post\" action=\"%s/category%s\">", CGI_PATH, EXECUTABLE ) ;

printf("<input type=hidden name=method value=\"insert\"> \n") ;

printf( "<input type=hidden name=category  value=\"%d\"> \n", category.id ) ;

printf("<table border=0>\n") ;

GetLocaleText ( MESSAGES, LOCALE, 3, sLocale ) ; /* Category */
printf("<tr><td>%s: </td><td valign=\"top\">", sLocale ) ;
TextBoxHTML ( "name",category.name, 25 ) ;
printf("</td></tr>") ;

GetLocaleText ( MESSAGES, LOCALE, 7, sLocale ) ; /* Text */
printf("<tr><td>%s: <td valign=\"top\">", sLocale ) ;
EditBoxHTML ( "remark", "", 5,35 ) ;
printf("</td></tr>") ;

printf("<tr><td></td><td>" ) ;
GetLocaleText ( MESSAGES, LOCALE, 10, sLocale ) ; /* Save */
printf("<input type=submit value=\"%s \">", sLocale ) ;
GetLocaleText ( MESSAGES, LOCALE, 11, sLocale ) ; /* Cancel */
printf("<input type=reset value=\"%s\">", sLocale ) ;
printf ("</td></tr></table>") ;
printf ("</FORM>" ) ;

}

/*

  Chose the appropriciate category for an object

 

*/

void printObjectCategoryForm ( struct _category category, char *program, char *sLocale )

{

printf("<H2>%s</H2>\n", sLocale ) ;  /* Name of form */

printf("<FORM method=\"post\" action=\"%s/%s%s\">\n", CGI_PATH, program, EXECUTABLE ) ;
printf("<input type=hidden name=method value=\"edit\"> \n") ;
printf("<input type=hidden name=subject value=\"0\"> \n") ;

printf("<table border=0>\n") ;
printf("<tr>") ;
GetLocaleText ( MESSAGES, LOCALE, 3, sLocale ) ; /* Category */
printf("<td>%s</td>", sLocale ) ;
printf("<td>") ;

selectCategory ( &category,"ALL","DATE", NULL ) ; /* läs önskade kategorier  från fil */

CategoryListBoxHTML ( &category ) ;

printf("</td></tr>\n") ;
printf("<tr><td></td><td>" ) ;
GetLocaleText ( MESSAGES, LOCALE, 16, sLocale ) ; /* Continue */
printf("<input type=submit value=\"Forts&auml;tt \">") ;
GetLocaleText ( MESSAGES, LOCALE, 11, sLocale ) ; /* Cancel */
printf("<input type=reset value=\"Avbryt\">") ;
printf ("</td></tr></table>") ;
printf ("</FORM>" ) ;

}

void printArticleCategoryForm ( struct _category category )

{

char sLocale[255] ;

GetLocaleText ( MESSAGES, LOCALE, 44, sLocale ) ; /* Cancel */
printObjectCategoryForm ( category, "article", sLocale ) ;

}

void printPublicationCategoryForm ( struct _category category )

{

char sLocale[255] ;

GetLocaleText ( MESSAGES, LOCALE, 74, sLocale ) ; /* Cancel */
printObjectCategoryForm ( category, "publication", sLocale ) ;

}
void printDocumentCategoryForm ( struct _category category )

{

char sLocale[255] ;

GetLocaleText ( MESSAGES, LOCALE, 74, sLocale ) ; /* Cancel */
printObjectCategoryForm ( category, "document", sLocale ) ;

}

void printMessageCategoryForm ( struct _category category )

{

char sLocale[255] ;

GetLocaleText ( MESSAGES, LOCALE, 29, sLocale ) ; /* Category for message */
printObjectCategoryForm ( category, "message", sLocale ) ;

}

void printCommentCategoryForm ( struct _category category )

{

char sLocale[255] ;

GetLocaleText ( MESSAGES, LOCALE, 69, sLocale ) ; /* Category for message */
printObjectCategoryForm ( category, "comment", sLocale ) ;

}

void printLinkCategoryForm ( struct _category category )

{

char sLocale[255] ;

GetLocaleText ( MESSAGES, LOCALE, 34, sLocale ) ; /* Category for link */
printObjectCategoryForm ( category, "link", sLocale ) ;

}



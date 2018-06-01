
/***************************************************************************
 Konferanssystem
 Program: %M%
 Version: %I%
 Date:    2006-08-24
     written by Casimir Artmann   
 (c) copyright Casimir Artmann. 1996-1998.
 ****************************************************************************/ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TRUE 1
#define FALSE 0 


struct _folder { 
        int id ;             /* nr pÂ fil */
		int category ;
		int subject ;
        char title[64] ;
		char author[64] ;
		char email[64] ;
		char filename[64] ;
		int  security  ;      /* publik eller privat */
        int  owner ;         /* ‰gare till nyhetsgruppen */
        int  group ;             /* */
		char created[26] ; /* datum fˆr senaste artikel */
		char remark[255] ;     
		struct _folder *next ;   /* n‰sta nyhetsgrupp */
		} ;

void catfHTML ( char *filename, FILE *file ) ;

int selectFolder ( struct _folder *folder, char *show, char *sort, char *cursor ) ;		
int printFolderHTML ( char *sCategory, char *sSubject, char *sFolder, struct _folder *folder,int pages, char *header, char *footer ) ;

main ()
{

char sCategory[255] ;
char sSubject[255] ;
char sFolder [255] ;

char header[255] ;
char footer [255] ;

char show[255] ;
char sort[255] ;
char cursor[255] ;
int pages ;

struct _folder folder ;

printf("Category: ") ;
scanf ("%s", sCategory ) ;

printf("Subject: ") ;
scanf("%s", sSubject ) ;

printf("Folder: ") ;
scanf("%s", sFolder ) ;

printf ("Header: ") ;
scanf ("%s", header ) ;

printf ("Footer: ") ;
scanf ("%s", footer ) ;

if (TRUE)
 {
  pages = selectFolder ( &folder,"ALL","", NULL ) ;
  if ( pages > 0 )
    printFolderHTML ( sCategory, sSubject, sFolder, &folder, pages, header, footer ) ; 
  return ( TRUE ) ;    
 }
               

}  /* end of main () */


void catfHTML ( char *filename, FILE *file) 

{

char string[255] ;
FILE *fp ;

if (( fp = fopen ( filename, "r" )) == NULL )
   {
    printf ( "<cite>Filen %s kan inte &ouml;ppnas.</cite><p>\n", filename ) ;
    exit ( 1 ) ;
   }

while ( fgets ( string, 255, fp ) != NULL )
   {
    fprintf (file, "%s", string ) ;
   }

fclose ( fp ) ;

}




selectFolder ( struct _folder *folder, char *show, char *sort, char *cursor )

   /* folder - l‰nkad lista med aktuella katagorier
      show      - ALL, visa alla inl‰gg
      sort      - DATE, sortera efter datum
      cursor    - NULL
         
   */

{

int i,j ;               /* räknare för strängar */
int counter ;           /* antal poster */
char filename[128] ;    /* filnamn för fil som innehåller nyhetsgrupper */
char string[512];       /* rad för namn */
char temp[20] ;         /* omvandling string -> heltal */

struct _folder *first ; /* första fil */
struct _folder *ptr ;  /* nästa fil */  

FILE *fp, *lock ;

filename[0] = (char) NULL ; 

   /* fins det en eller flera katagorier */

sprintf( filename, "folder.lck" ) ; 

if (( lock = fopen ( filename, "w" )) == NULL )
   {
    sprintf ( string, "Folder %s kan ej skrivas.", filename ) ;
    /* errorHTML ( string ) ; */
    return ( -1 ) ;
   }
    /* ˆppna filen fˆr l‰sning */

sprintf( filename, "images.dat") ; 

if (( fp = fopen ( filename, "r" )) == NULL )
   {
    sprintf ( string, "Folder %s kan ej l‰sas.", filename ) ;
    /* errorHTML ( string ) ;*/
    return ( -1 ) ;
   }

   /* adressen till fˆrsta posten blir startadress fˆr en l‰nkad lista */
   
counter = 0 ;
folder->next = NULL ;
first = folder ;

  /* l‰s rubriker frÂn fil */

while ( fgets ( string, 512, fp ) != NULL )
 {
     /* skapa minne fˆr en ny nyhetsgrupp */
     /* fˆrsta posten finns redan allokerad */  

   if ( counter > 0 )
    {  
     ptr = malloc ( sizeof ( struct _folder )) ;  /* adress till n‰sta post */
     ptr->next = NULL ;                             /* efterfˆljande adress saknas */
     folder->next = ptr ;                        /* l‰gg till adress till befintlig post */
     folder = ptr ;                              /* ny adress fˆr post */  

     } 
  
  counter ++ ;                      /* antal poster */
		
   /* plocka fram id fˆr varje fil */ 
   
  folder->id = counter ;
   
  /* plocka fram rubrik för varje fil */

  for (i=0,j=0;i<(int)strlen(string);i++,j++)
   {
    if ( string[i] == '\t' )
      break ;      
    folder->title[j] = string[i] ;
   }

   folder->title[j] = (char) NULL ;
   i++ ;
 
	
 /* plocka fram författare för varje fil */

  for (j=0;i<(int)strlen(string);i++,j++)
   {
    if ( string[i] == '\t' )
      break ;      
    folder->author[j] = string[i] ;
   }

   folder->author[j] = (char) NULL ;
   i++ ;    

     /* plocka fram e-mail för varje fil */

  for (j=0;i<(int)strlen(string);i++,j++)
   {
    if ( string[i] == '\t' )
      break ;      
    folder->email[j] = string[i] ;
   }

   folder->email[j] = (char) NULL ;
   i++ ;    

  /* plocka fram filnamn för varje fil */

  for (j=0;i<(int)strlen(string);i++,j++)
   {
    if ( string[i] == '\t' )
      break ;      
    folder->filename[j] = string[i] ;
   }

   folder->filename[j] = (char) NULL ;
   i++ ;    
}

fclose ( fp ) ;
fclose ( lock ) ;
return ( counter ) ;  

} /* end of select folder */



int printFolderHTML ( char *sCategory, char *sSubject, char *sFolder, struct _folder *folder, int pages, char *header, char *footer) 

{

int counter ;

struct _folder *first ;
char filename[128] ;    /* filnamn fˆr fil som innehÂller nyhetsgrupper */
char string[512];       /* rad fˆr namn */

FILE *fp, *page ;

first = folder ;
counter = 0 ; 

filename[0] = (char) NULL ; 

   /* fins det en eller flera katagorier */

sprintf( filename, "index.html"  ) ; 

if (( fp = fopen ( filename, "w" )) == NULL )
   {
    sprintf ( string, "Filen %s kan ej skapas.", filename ) ;
    /* errorHTML ( string ) ; */
    return ( -1 ) ;
   }

catfHTML ( header, fp ) ;
     
fprintf(fp, "<H3>%s - %s - %s</H3><br>\n", sCategory, sSubject, sFolder ) ;
fprintf(fp, "<P>") ;

while ( folder != NULL )
 {
  fprintf( fp, "<a href=\"page_%d.html\">%s</a> %s", folder->id, folder->title, folder->author ) ;
  fprintf( fp, "<br>\n") ;
  
  sprintf( filename, "page_%d.html", folder->id ) ; 

  if (( page = fopen ( filename, "w" )) == NULL )
   {
    sprintf ( string, "Sida %s kan ej skapas.", filename ) ;
    /* errorHTML ( string ) ; */
    return ( -1 ) ;
   }
  
  catfHTML ( header, page ) ;
    
  fprintf(page, "<p>\n" ) ;
  fprintf(page, "<table><tr>\n") ;
  
  fprintf(page, "<td width=\"600\" height=\"600\"><img src=\"%s\"\n</td>", folder->filename ) ;
  fprintf(page, "<p></tr><tr>\n" ) ;
  fprintf(page, "<td>%s (c) %s<br>\n</td>", folder->title, folder->author, folder->created ) ;
  fprintf(page, "</tr><tr><td>%s<p></td>\n", folder->remark ) ;
  fprintf(page, "<p></tr></table>\n" ) ;
  
  if ( folder->id == 1) 
    fprintf(page, " <a href=\"index.html\">[ ^ %s ]</a> - [ << F&ouml;rsta ] - [ < F&ouml;reg&aring;ende ] - <a href=\"page_%d.html\">[  N&auml;sta > ]</a> - <a href=\"page_%d.html\">[ Sista >> ]", sSubject, folder->id+1, pages) ;
	
  if (( folder->id > 1) && ( folder->id < pages))  
    fprintf(page, " <a href=\"index.html\">[ ^ %s ]</a> <a href=\"page_1.html\">[ << F&ouml;rsta ]</a> - <a href=\"page_%d.html\">[ < F&ouml;reg&aring;ende ]</a> - <a href=\"page_%d.html\">[ N&auml;sta > ]</a> - <a href=\"page_%d.html\">[ Sista >> ]</a>", sSubject, folder->id-1, folder->id+1, pages ) ;

  if ( folder->id == pages) 
    fprintf(page, " <a href=\"index.html\">[ ^ %s ]</a> <a href=\"page_1.html\">[ << F&ouml;rsta ]</a> - <a href=\"page_%d.html\">[ < F&ouml;reg&aring;ende ]</a> - [  N&auml;sta > ] - [ Sista >> ]", sSubject,folder->id-1) ;
	
  fprintf(page, "<p>\n" ) ;
  catfHTML ( footer, page ) ;
  
  fclose ( page ) ;
  
  folder = folder->next ;
  counter ++ ;
  
 }
 
folder = first ;

catfHTML ( footer, fp ) ;

fclose (fp) ;

} /* end of printFolderHTML */











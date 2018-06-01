
/***********************************************************************

 Database conversion proigram for Intra 2.1.3 to 2.1.3 

 Program: %M%
 Version: %I%
 Date:    %D%

*************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>



#include "../cgi.h"

#include "../utils.h"

#include "intra.h"
#include "db.h"
#include "html.h"

int readGroupTitle ( char *string, struct _title *title ) ;

main ( int argc, char *argv[] )

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

char sFromFilename[128] ;    /* filnamn f�r fil som inneh�ller ett inl�gg */
char sToFilename[128] ;
char filename[128] ;

char string[255];       /* rad f�r rubrik */
char path[255] ;

char sName[40] ;

int skip ;                 /* hoppa �ver posten */
               
FILE *fp, *new_fp, *lock, *new_lock, *new_id, *id ;

struct _title title ;    /* inl�st post */

strcpy ( path, argv[0] ) ;
InitIntra ( path ) ; 

/* InitCGI () ; */

sFromFilename[0] = (char) NULL ;
sToFilename[0] = (char) NULL ;

skip = FALSE ;
counter = 0 ;
         
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

 /* skirv ny l�sfil */
 
 /* kopiera r�knare */
 
 
    /* �ppna filen f�r l�sning */

if ( strcmp( EDITION, "Limited"))
  sprintf( sFromFilename, "%s/news-%d-%d.dat", DB_PATH, category_id, subject_id ) ; 
else
  sprintf( sFromFilename, "%s/news-%d.dat", DB_PATH, subject_id ) ; 

if ( strcmp( EDITION, "Limited"))
  sprintf( sToFilename, "%s/subject-%d-%d.dat", DB_PATH, category_id, subject_id ) ; 
else
  sprintf( sToFilename, "%s/subject-%d.dat", DB_PATH, subject_id ) ; 

if (( fp = fopen ( sFromFilename, "r" )) == NULL )
   {
    sprintf ( string, "Rubriker %s kan ej l�sas.", sFromFilename ) ;
    errorHTML ( string ) ;
    return ( -1 ) ;
   }

if (( new_fp = fopen ( sToFilename, "w" )) == NULL )
   {
    sprintf ( string, "Rubriker %s kan ej skrivas.", sToFilename ) ;
    errorHTML ( string ) ;
    return ( -1 ) ;
   }

  /* l�s rubriker fr�n fil */

while ( fgets ( string, 1024, fp ) != NULL )
  {                                              

	readGroupTitle ( string, &title ) ;

    fprintf( fp, "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s\t%s\t", title.id, title.subject, title.subclass, title.security, title.owner, title.group, title.indent, title.status, title.created, title.modified ) ;
    fprintf( new_fp, "%d\t%d\t%s\t%s\t%s\t%s\n",  title.object, title.language, title.name, title.author, title.email, title.company ) ;
	   
  } /* end of while */


fclose ( fp ) ;
fclose ( new_fp ) ;
fclose ( lock ) ;
 
   }

 }  /*  */

return ( TRUE ) ;
 
} /* end of main */




int readGroupTitle ( char *string, struct _title *title ) 

{

int i, j ;               /* r�knare f�r str�ngar */


char temp[20] ;         /* omvandling string -> heltal */

  for (i=0;i<(int)strlen(string);i++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[i] = string[i] ;
   }
  temp[i] = (char) NULL ;
  title->id = atoi ( temp ) ;
  i++ ;
 
  /* aktuellt �mne */

  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;
  title->subject = atoi ( temp ) ;
  i++ ;
  
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;
  title->security = atoi ( temp ) ;
  i++ ;                         
  
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;
  title->owner = atoi ( temp ) ;
  i++ ;
  
     /* niv� p� indrag */
     
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;
  title->indent = atoi ( temp ) ;
  i++ ;

    /* plocka fram tidst�mpling f�r varje rubrik */

   for (j=0;i<(int)strlen(string);i++,j++)
    {
    if ( string[i] == '\t' )
      break ;      
    title->created[j] = string[i] ;
    }
   title->created[j] = (char) NULL ;
   i++ ;        
   
     /* dokumenttyp  1 - HTML l�nk, 2 - Article, 3 - Message, 4 - Dokumentl�nk */
     
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;
  title->object = atoi ( temp ) ;
  i++ ;
                              
    /* group */
	
  for (j=0;i<(int)strlen(string);i++,j++)
    {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
    }
   temp[j] = (char) NULL ;
   title->object = atoi ( temp ) ;
   i++ ;

   /* plocka fram titel f�r varje rubrik */

   for (j=0;i<(int)strlen(string);i++,j++)
    {
    if ( string[i] == '\t' )
      break ;      
    title->name[j] = string[i] ;
    }
   title->name[j] = (char) NULL ;
   i++ ;

    /* plocka fram e-post f�r varje rubrik */

   for (j=0;i<(int)strlen(string);i++,j++)
    {
    if ( string[i] == '\t' )
      break ;      
    title->email[j] = string[i] ;
    }
   title->email[j] = (char) NULL ;
   i++ ;        
           
    /* plocka fram namn f�r varje rubrik */

   for (j=0;i<(int)strlen(string);i++,j++)
    {
    if ( string[i] == '\t' )
      break ;      
    title->author[j] = string[i] ;
    }
   title->author[j] = (char) NULL ;
   i++ ;        

    /* hoppa �ver f�retag f�r varje rubrik */

   for (j=0;i<(int)strlen(string);i++,j++)
    {
    if ( string[i] == '\t' )
      break ;      
    }
   i++ ;        

   title->company[0] = (char) NULL ;

   return ( TRUE ) ;

}  /* end of readTitle */ 


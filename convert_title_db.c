
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

   /* subject - länkad lista med aktuella rubriker
      show    - UNREAD, visa alla inlägg
      sort    - DATE, sortera efter datum
      cursor  - NULL
         
   */

{

int category_id ;               /* räknare för strängar */
int subject_id ; 
int counter ;           /* antal lästa poster */
int groups ;            /* vilken nyhetsgrupp som avses */
int categories ;        /* vilka rubriker som finns */

char sFromFilename[128] ;    /* filnamn för fil som innehåller ett inlägg */
char sToFilename[128] ;
char filename[128] ;

char string[255];       /* rad för rubrik */
char path[255] ;

char sName[40] ;

int skip ;                 /* hoppa över posten */
               
FILE *fp, *new_fp, *lock, *new_lock, *new_id, *id ;

struct _title title ;    /* inläst post */

strcpy ( path, argv[0] ) ;
InitIntra ( path ) ; 

/* InitCGI () ; */

sFromFilename[0] = (char) NULL ;
sToFilename[0] = (char) NULL ;

skip = FALSE ;
counter = 0 ;
         
/* läs antal katagorier */ 
 
  categories = getLastCategoryId () ;

  for (category_id=1;category_id<=categories;category_id++)   /* för varje katagori */
   {
  
   /* läs antal ämnen för aktuell kategori */
    groups = getLastSubjectId ( category_id ) ;

    if ( groups == 0 )
     continue ;

    for (subject_id=1;subject_id<=groups;subject_id++)
     {
    
      /* Om valt ämne saknas skall man hoppa över detta ämne */
      if ( !getSubjectName ( category_id,subject_id,sName ))
        continue ;
     
 
  /* sätt värden för aktuell kategori / ämne */


  /* Om låsfilen inte går att öppna skall man inte fortsätta */

if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/news-%d-%d.lck", DB_PATH, category_id, subject_id ) ; 
else
  sprintf( filename, "%s/news-%d.lck", DB_PATH, subject_id ) ; 

if (( lock = fopen ( filename, "w" )) == NULL )
   {
    sprintf ( string, "Rubriker %s kan ej läsas.", filename ) ;
    errorHTML ( string ) ;
    return ( -1 ) ;
   }

 /* skirv ny lŒsfil */
 
 /* kopiera rŠknare */
 
 
    /* öppna filen för läsning */

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
    sprintf ( string, "Rubriker %s kan ej läsas.", sFromFilename ) ;
    errorHTML ( string ) ;
    return ( -1 ) ;
   }

if (( new_fp = fopen ( sToFilename, "w" )) == NULL )
   {
    sprintf ( string, "Rubriker %s kan ej skrivas.", sToFilename ) ;
    errorHTML ( string ) ;
    return ( -1 ) ;
   }

  /* läs rubriker från fil */

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

int i, j ;               /* räknare för strängar */


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
 
  /* aktuellt ämne */

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
  
     /* nivå på indrag */
     
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;
  title->indent = atoi ( temp ) ;
  i++ ;

    /* plocka fram tidstämpling för varje rubrik */

   for (j=0;i<(int)strlen(string);i++,j++)
    {
    if ( string[i] == '\t' )
      break ;      
    title->created[j] = string[i] ;
    }
   title->created[j] = (char) NULL ;
   i++ ;        
   
     /* dokumenttyp  1 - HTML länk, 2 - Article, 3 - Message, 4 - Dokumentlänk */
     
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

   /* plocka fram titel för varje rubrik */

   for (j=0;i<(int)strlen(string);i++,j++)
    {
    if ( string[i] == '\t' )
      break ;      
    title->name[j] = string[i] ;
    }
   title->name[j] = (char) NULL ;
   i++ ;

    /* plocka fram e-post för varje rubrik */

   for (j=0;i<(int)strlen(string);i++,j++)
    {
    if ( string[i] == '\t' )
      break ;      
    title->email[j] = string[i] ;
    }
   title->email[j] = (char) NULL ;
   i++ ;        
           
    /* plocka fram namn för varje rubrik */

   for (j=0;i<(int)strlen(string);i++,j++)
    {
    if ( string[i] == '\t' )
      break ;      
    title->author[j] = string[i] ;
    }
   title->author[j] = (char) NULL ;
   i++ ;        

    /* hoppa över företag för varje rubrik */

   for (j=0;i<(int)strlen(string);i++,j++)
    {
    if ( string[i] == '\t' )
      break ;      
    }
   i++ ;        

   title->company[0] = (char) NULL ;

   return ( TRUE ) ;

}  /* end of readTitle */ 


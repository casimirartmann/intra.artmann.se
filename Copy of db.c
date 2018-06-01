
/******************************************************************************************
 

   Program: %M%
   Version:   %I%
   Date:       %D% 
  
   written by Casimir Artmann. 1996-1998

   findUser
   checkCategory 
   findNewsgroup
   findCategory

   deleteSubject 
   deleteArticle

   insertSubject    - sparar nyhetsgrupp
   insertArticle    - sparar en ny artikel med tillh�rande rubrik

   selectSubject    - l�ser aktuella nyhetsgrupper
   selectTitle      - l�ser aktuella rubriker f�r en nyhetsgrupp
   selectArticle    - l�ser en eller flera artiklar

   #include "../cgi.h"      - errorHTML ( char *string )
   #include "intranet.h" - 
   #include "db.h"          -
   
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../cgi.h"
#include "../utils.h"
#include "intra.h"
#include "db.h"


/* h�mta fram information om en anv�ndare baserat p� inloggningsnamnet */

findUser ( char *name, struct _user *user )

{

FILE *fp ;
int i,j ;

char temp[16] ;
char string [512 ] ; 
char filename [ 128] ;

user->id = -1 ;     /* default v�rde om anv�ndare saknas */

if ( strlen ( name ) == 0 )
  return ( FALSE ) ;

sprintf( filename, "%s/user.dat" , DB_PATH ) ;
if (( fp = fopen ( filename, "r" )) == NULL )
   {
    errorHTML ("Kan inte l�sa information om anv�ndare") ;
    return ( ERROR ) ;
   }

while ( fgets ( string, 512, fp ) != NULL )
 {
  /* plocka fram id f�r varje anv�ndare   */ 
   
  for (i=0;i<(int)strlen(string);i++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[i] = string[i] ;
   }
  temp[i] = (char) NULL ;
  user->id = atoi ( temp) ;
  i++ ;        

 for (j=0;i<(int)strlen(string);i++,j++)
    {
    if ( string[i] == '\t' )
      break ;      
    user->login[j] = string[i] ;
    }
   user->login[j] = (char) NULL ;
   i++ ;     
 
  for (j=0;i<(int)strlen(string);i++,j++)
    {
    if ( string[i] == '\t' )
      break ;      
    user->name[j] = string[i] ;
    }
   user->name[j] = (char) NULL ;
   i++ ;     

   for (j=0;i<(int)strlen(string);i++,j++)
    {
    if ( string[i] == '\t' )
      break ;      
    user->epost[j] = string[i] ;
    }
   user->epost[j] = (char) NULL ;
   i++ ;     

  for (j=0;i<(int)strlen(string);i++,j++)
    {
    if ( string[i] == '\t' )
      break ;      
    user->company[j] = string[i] ;
    }
   user->company[j] = (char) NULL ;
   i++ ;     

  for (j=0;i<(int)strlen(string);i++,j++)
    {
    if ( string[i] == '\t' )
      break ;      
    user->homepage[j] = string[i] ;
    }
   user->homepage[j] = (char) NULL ;
   i++ ;     
  if ( !strcmp ( name, user->login ))
   return ( TRUE ) ;
 
 }

user->name[0] = (char) NULL ;
user->homepage[0] = (char) NULL ;
user->epost[0] = (char) NULL ;
user->company[0] = (char) NULL ;

return ( FALSE ) ;

} /* end of findUser */

  /* kontrollerar om det finns skapade katagorier */

checkCategory () 

{

char filename[255] ;
char logname [255] ;

FILE *fp, *log ;

sprintf( filename, "%s/category.dat", DB_PATH  ) ;
sprintf( logname, "%s/category.id", DB_PATH ) ;

/* om b�de category.dat och category.id saknas s� skall dessa filer skapas */
if ( (( fp = fopen ( filename, "r")) == NULL ) &&
     (( log = fopen ( logname, "r" )) == NULL ))

  {
   
   fp = fopen ( filename,"w" ) ;
   log = fopen ( logname,"w" ) ;  

   fputs ("0", log ) ;

   fclose ( fp ) ;
   fclose ( log ) ;
  }

return ( TRUE ) ;

}

int getLastCategoryId ( )

{

int id ;

char filename[255];
char string[255];

FILE  *log ;

	if ( strcmp (EDITION, "Limited"))   /* Standard and extended edition */
      {
       sprintf( filename, "%s/category.id", DB_PATH  ) ; 

       if (( log = fopen ( filename, "r" )) == NULL )
         {
          printf("Limited edition\n<br>") ;
		  return ( 0 ) ;
         } 

   if ( fgets ( string, 512, log ) != NULL )          /* l�s f�rsta fr�n fil som inneh�ller l�pnummer */
     id = atoi ( string ) ;    /* senaste l�pnummer */

  fclose ( log ) ;

  return ( id ) ;

	}

return ( FALSE ) ;


}

int getLastSubjectId ( int category )

{

int id ;

char filename[255];
char string[255];
FILE  *log ;

sprintf( filename, "%s/group-%d.id", DB_PATH, category  ) ;
  
    if (( log = fopen ( filename, "r" )) == NULL )
     {
      errorHTML ("Nyhetsgruppen kan ej l�sas. Fel vid l�sning av id") ;
      printf("%s", filename ) ;

      return ( -1 ) ;
     }
 
    string[0] = (char) NULL ;
    if ( fgets ( string, 512, log ) != NULL )          /* l�s f�rsta fr�n fil som inneh�ller l�pnummer */
      id = atoi ( string ) ;    /* senaste l�pnummer */
 
    fclose ( log ) ;

	return ( id ) ;
} 

/* 




*/

getSubjectName ( int category, int group, char *name ) 

{

int i,j ;               /* r�knare f�r str�ngar */
int id ;              /* aktuell grupp */
char filename[128] ;    /* filnamn f�r fil som inneh�ller nyhetsgrupper */
char string[512];       /* rad f�r namn */
char temp[255] ;         /* omvandling string -> heltal */

FILE *fp, *lock ;

filename[0] = (char) NULL ; 

   /* finns det en eller flera katagorier */

if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/group-%d.lck", DB_PATH, category) ; 
else
  sprintf( filename, "%s/group.lck", DB_PATH ) ; 

if (( lock = fopen ( filename, "w" )) == NULL )
   {
    sprintf ( string, "Nyhetsgrupper %s kan ej l�sas.", filename ) ;
    errorHTML ( string ) ;
    return ( -1 ) ;
   }

    /* �ppna filen f�r l�sning */

if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/group-%d.dat", DB_PATH, category ) ; 
else
  sprintf( filename, "%s/group.dat" , DB_PATH ) ; 

if (( fp = fopen ( filename, "r" )) == NULL )
   {
    sprintf ( string, "Nyhetsgrupper %s kan ej l�sas.", filename ) ;
    errorHTML ( string ) ;
    return ( -1 ) ;
   }


  /* l�s nyhetsgrupper fr�n fil */

while ( fgets ( string, 512, fp ) != NULL )
 {

   /* plocka fram id f�r varje nyhetsgrupp */ 
   
  for (i=0;i<(int)strlen(string);i++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[i] = string[i] ;
   }
  temp[i] = (char) NULL ;
  id = atoi ( temp ) ;
  i++ ;
      /* om inte r�tt grupp s� n�sta rad */

 if ( id != group )  
  continue ;

    /* owner */
  
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;
  i++ ;      
   
    /* protect */
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;

  i++ ;      
   
   /* senaste lagrade artikel*/
   
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;

  i++ ;      
  
   /* datumm�rkning, YYYY-MM-DD hh:mm:ss */
   
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;
  i++ ;      
  
   /* plocka fram namn f�r varje nyhetsgrupp */

  for (j=0;i<(int)strlen(string);i++,j++)
   {
    if ( string[i] == '\t' )
      break ;      
    name[j] = string[i] ;
   }

   name[j] = (char) NULL ;

   fclose ( fp ) ;
   fclose ( lock ) ;
   return ( TRUE ) ;
 
  }

fclose ( fp ) ;
fclose ( lock ) ;
return ( FALSE ) ;  

}  /* end of getSubjectName **/

getSubjectClass ( int category, int group ) 

{

int i,j ;               /* r�knare f�r str�ngar */
int id ;              /* aktuell grupp */
char iInformationClass ;

char filename[128] ;    /* filnamn f�r fil som inneh�ller nyhetsgrupper */
char string[512];       /* rad f�r namn */
char temp[255] ;         /* omvandling string -> heltal */

FILE *fp, *lock ;

filename[0] = (char) NULL ; 

   /* finns det en eller flera katagorier */

if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/group-%d.lck", DB_PATH, category) ; 
else
  sprintf( filename, "%s/group.lck", DB_PATH ) ; 

if (( lock = fopen ( filename, "w" )) == NULL )
   {
    sprintf ( string, "Nyhetsgrupper %s kan ej l�sas.", filename ) ;
    errorHTML ( string ) ;
    return ( -1 ) ;
   }

    /* �ppna filen f�r l�sning */

if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/group-%d.dat", DB_PATH, category ) ; 
else
  sprintf( filename, "%s/group.dat" , DB_PATH ) ; 

if (( fp = fopen ( filename, "r" )) == NULL )
   {
    sprintf ( string, "Nyhetsgrupper %s kan ej l�sas.", filename ) ;
    errorHTML ( string ) ;
    return ( -1 ) ;
   }

  /* l�s nyhetsgrupper fr�n fil */

while ( fgets ( string, 512, fp ) != NULL )
 {

   /* plocka fram id f�r varje nyhetsgrupp */ 
   
  for (i=0;i<(int)strlen(string);i++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[i] = string[i] ;
   }
  temp[i] = (char) NULL ;
  id = atoi ( temp ) ;
  i++ ;
      /* om inte r�tt grupp s� n�sta rad */

 if ( id != group )  
  continue ;

    /* owner */
  
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;
  i++ ;      
   
    /* protect */
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;
  
  iInformationClass = atoi ( temp) ;
  
  }

fclose ( fp ) ;
fclose ( lock ) ;
return ( iInformationClass ) ;  

}  /* end of getSubjectClass **/



getSubjectRemark ( int category, int group, char *name ) 

{

int i,j ;               /* r�knare f�r str�ngar */
int id ;              /* aktuell grupp */
char filename[128] ;    /* filnamn f�r fil som inneh�ller nyhetsgrupper */
char string[512];       /* rad f�r namn */
char temp[255] ;         /* omvandling string -> heltal */

FILE *fp, *lock ;

filename[0] = (char) NULL ; 

   /* finns det en eller flera katagorier */

if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/group-%d.lck", DB_PATH, category) ; 
else
  sprintf( filename, "%s/group.lck", DB_PATH ) ; 

if (( lock = fopen ( filename, "w" )) == NULL )
   {
    sprintf ( string, "Nyhetsgrupper %s kan ej l�sas.", filename ) ;
    errorHTML ( string ) ;
    return ( -1 ) ;
   }

    /* �ppna filen f�r l�sning */

if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/group-%d.dat", DB_PATH, category ) ; 
else
  sprintf( filename, "%s/group.dat" , DB_PATH ) ; 

if (( fp = fopen ( filename, "r" )) == NULL )
   {
    sprintf ( string, "Nyhetsgrupper %s kan ej l�sas.", filename ) ;
    errorHTML ( string ) ;
    return ( -1 ) ;
   }


  /* l�s nyhetsgrupper fr�n fil */

while ( fgets ( string, 512, fp ) != NULL )
 {

   /* plocka fram id f�r varje nyhetsgrupp */ 
   
  for (i=0;i<(int)strlen(string);i++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[i] = string[i] ;
   }
  temp[i] = (char) NULL ;
  id = atoi ( temp ) ;
  i++ ;
      /* om inte r�tt grupp s� n�sta rad */

 if ( id != group )  
  continue ;

    /* owner */
  
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;
  i++ ;      
   
    /* protect */
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;

  i++ ;      
   
   /* senaste lagrade artikel*/
   
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;

  i++ ;      
  
   /* datumm�rkning, YYYY-MM-DD hh:mm:ss */
   
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;
  i++ ;      
  
    /* plocka fram namn */

  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;
  i++ ;      

   /* plocka fram f�rklarande text f�r varje nyhetsgrupp */

  for (j=0;i<(int)strlen(string);i++,j++)
   {
    if ( string[i] == '\t' )
      break ;      
    name[j] = string[i] ;
   }

   name[j] = (char) NULL ;

   fclose ( fp ) ;
   fclose ( lock ) ;
   return ( TRUE ) ;
 
  }

fclose ( fp ) ;
fclose ( lock ) ;
return ( FALSE ) ;  

}  /* end of getSubjectRemark **/

getCategoryName ( int category, char *name ) 

{

int i,j ;               /* r�knare f�r str�ngar */
int id ;              /* aktuell grupp */
char filename[128] ;    /* filnamn f�r fil som inneh�ller kategorier  */
char string[512];       /* rad f�r namn */
char temp[255] ;         /* omvandling string -> heltal */

FILE *fp, *lock ;

filename[0] = (char) NULL ; 

   /* finns det en eller flera katagorier */

if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/group-%d.lck", DB_PATH, category) ; 
else
  sprintf( filename, "%s/group.lck", DB_PATH ) ; 

if (( lock = fopen ( filename, "w" )) == NULL )
   {
    sprintf ( string, "Nyhetsgrupper %s kan ej l�sas.", filename ) ;
    errorHTML ( string ) ;
    return ( -1 ) ;
   }

    /* �ppna filen f�r l�sning */

 sprintf( filename, "%s/category.dat" , DB_PATH ) ; 

if (( fp = fopen ( filename, "r" )) == NULL )
   {
    sprintf ( string, "Kategorier %s kan ej l�sas.", filename ) ;
    errorHTML ( string ) ;
    return ( -1 ) ;
   }


  /* l�s kategorier fr�n fil */

while ( fgets ( string, 512, fp ) != NULL )
 {

   /* plocka fram id f�r varje kategori  */ 
   
  for (i=0;i<(int)strlen(string);i++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[i] = string[i] ;
   }
  temp[i] = (char) NULL ;
  id = atoi ( temp ) ;
  i++ ;
      /* om ine r�tt kategori s� n�sta rad */

 if ( id != category  )  
  continue ;

    /* owner */
  
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;
  i++ ;      
   
    /* protect */
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;

  i++ ;      
   
   /* senaste lagrade nyhetsgrupp */
   
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;

  i++ ;      
  
   /* datumm�rkning, YYYY-MM-DD hh:mm:ss */
   
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;
  i++ ;      
  
   /* plocka fram namn f�r varje kategori  */

  for (j=0;i<(int)strlen(string);i++,j++)
   {
    if ( string[i] == '\t' )
      break ;      
    name[j] = string[i] ;
   }

   name[j] = (char) NULL ;

   fclose ( fp ) ;
   fclose ( lock ) ;
   return ( TRUE ) ;
 
  }

fclose ( fp ) ;
fclose ( lock ) ;
return ( FALSE ) ;  


}  /* */

/******************************************************************

 S�tter ett l�s f�r skrivning till angiven kategori/�mne

 l�sningsfilen news-?-?.lck resp news-?.lck �ppnas f�r skrivning
 om filen inte kan �ppnas f�r skrivning returneras FALSE, 
 i annat fall returneras TRUE.

*******************************************************************/

FILE *lockSubject ( int x, int y ) 

{

char filename[128] ;

FILE *lock ;

	if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/news-%d-%d.lck", DB_PATH, x, y ) ;
else
  sprintf( filename, "%s/news-%d.lck", DB_PATH, y ) ;

if (( lock = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas. Fel vid postl�sning") ;
    return ( FALSE ) ;
   }

return ( lock ) ;

} /* end of lockSubject */


/******************************************************************

 L�ser upp f�r skrivning f�r andra processer till angiven kategori/�mne

 l�sningsfilen news-?-?.lck resp news-?.lck st�ngs
 om filen inte kan st�ngas returneras FALSE, 
 i annat fall returneras TRUE.

*******************************************************************/

int unlockSubject ( FILE *lock ) 

{

fclose ( lock ) ;

return ( TRUE ) ;

} /* end of unlockSubject */


int GetNextTitleId ( int x, int y )

{

char filename[128] ;    /* filnamn f�r fil som inneh�ller ett inl�gg */
char string[512] ;      /* st�ng f�r l�sning till och fr�n textfil */
int id ;

FILE  *log ;

if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/news-%d-%d.id", DB_PATH, x, y ) ;
else
  sprintf( filename, "%s/news-%d.id", DB_PATH, y ) ;

if (( log = fopen ( filename, "r" )) == NULL )
   {
    printf ("%s", filename ) ;
    errorHTML ("Inl�gget kan ej sparas. Fel vid l�sning av id " ) ;
    return ( -1 ) ;
   }
   
if ( fgets ( string, 512, log ) != NULL )  /* l�s tal fr�n str�ng */
 {
  id = atoi ( string ) ;
  id ++ ;                        /* �ka tal med ett */ 
 } 

    /* st�ng filen f�r n�sta nummer 		*/

fclose ( log ) ;

if (( log = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas. Fel vid l�sning av id") ;
    return ( -1 ) ;
   }

fprintf ( log, "%d", id ) ;
fclose ( log ) ;

return ( id ) ;

}

/******************************************************************

 Skriver ned rubrik om ett objekt i en fil 
 Innan funktionen anroas skall kategori/�mne vara l�st via funktionen
 lockSubject och efter�t skall uppl�sning g�ras via unlockSubject 
 
 Inl�ggen lagras i omv�nd krologisk ordning, med det senaste 
 inl�gget f�rs. Kommentarer l�ggs efter det inl�gg som den h�nvisar till, 
 och �ven dessa i omv�nd krologisk ordning.


*******************************************************************/

int insertTitle ( struct _title title ) 

{

char filename[128] ;    /* filnamn f�r fil som inneh�ller ett inl�gg */
char tmpname[128] ;
char string[512] ;      /* st�ng f�r l�sning till och fr�n textfil */
int id ;

time_t today ;

FILE *tmp, *group ;

if ( strcmp( EDITION, "Limited"))
{
	sprintf( filename, "%s/news-%d-%d.dat", DB_PATH, title.category, title.subject ) ;
	sprintf( tmpname, "%s/news-%d-%d.tmp", DB_PATH, title.category, title.subject ) ;
}
else
{
  sprintf( filename, "%s/news-%d.dat", DB_PATH, title.subject ) ;
  sprintf( tmpname, "%s/news-%d.tmp", DB_PATH, title.subject ) ;
}

if (( group = fopen ( filename, "r" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

if (( tmp = fopen ( tmpname, "w" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

/* skapa en kopia */

while ( fgets ( string, 512, group ) != NULL )
  fputs ( string, tmp  ) ;

fclose ( tmp ) ;
fclose ( group ) ;

/* */

if (( group = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

if (( tmp = fopen ( tmpname, "r" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

  /* h�mta aktuell tid */

time( &today ) ;
strftime( title.timestamp, 26,"%Y-%m-%d %H:%M:%S", localtime(&today) ) ;

  /* om niv� 0 p� bojekt s� spara f�rst i filen */

if ( title.indent == 0 )
  {
    fprintf( group, "%d\t%d\t%d\t%d\t%d\t", title.id, title.subject, 
        title.protect, title.owner, title.indent ) ;
    fprintf( group, "%s\t%d\t%d\t%s\t%s\t%s\t%s\n", title.timestamp, title.type,
         title.group, title.title, title.from, title.author, title.company ) ;
                 
    /* st�ng filen f�r rubriker */

  while ( fgets ( string, 512, tmp ) != NULL )
    fputs ( string, group  ) ;
  }
else
  {
   while ( fgets ( string, 512, tmp ) != NULL )
     {
       

      id = atoi ( string ) ;    /* forsta posten inneh�ller id */
      fputs ( string, group  ) ;

       if ( title.comment == id )    /* kommentar till detta inl�gg */
        {
          fprintf( group, "%d\t%d\t%d\t%d\t%d\t", title.id, title.subject, 
                        title.protect, title.owner, title.indent) ;
          fprintf( group, "%s\t%d\t%s\t%s\t%s\t%s\t%s\n", title.timestamp, title.type,
                      title.group, title.title, title.from, title.author, title.company ) ;
        }
     }
  }

fclose ( tmp ) ;
fclose ( group ) ;

return ( TRUE ) ;

} /* end of insertTitle */

/******************************************************************

 Uppdatera en befintlig rubrik om ett objekt i en fil 

 Innan funktionen anropas skall kategori/�mne vara l�st via funktionen
 lockSubject och efter�t skall uppl�sning g�ras via unlockSubject 

 Kategori eller �mne kan inte �ndras.

*******************************************************************/

int updateTitle ( struct _title title ) 

{

char filename[128] ;    /* filnamn f�r fil som inneh�ller ett inl�gg */
char tmpname[128] ;
char string[512] ;      /* st�ng f�r l�sning till och fr�n textfil */
char temp[8] ;
int i, id ;

FILE *tmp, *group ;

    /* spara ned id, rubrik, f�rfattare och timestamp i fil */        

if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/news-%d-%d.dat", DB_PATH, title.category, title.subject ) ;
else
  sprintf( filename, "%s/news-%d.dat", DB_PATH, title.subject ) ;

sprintf( tmpname, "%s/news-%d.tmp", DB_PATH, title.subject ) ;

if (( group = fopen ( filename, "r" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

if (( tmp = fopen ( tmpname, "w" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

/* skapa en kopia */

while ( fgets ( string, 512, group ) != NULL )
{
    fputs ( string, tmp  ) ;
}

fclose ( tmp ) ;
fclose ( group ) ;

/* */

if (( group = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

if (( tmp = fopen ( tmpname, "r" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

 /* l�s fram till aktuellt inl�gg */

while ( fgets ( string, 512, tmp ) != NULL )
{
 
  /* plocka fram id f�r varje rubrik */

  for (i=0;i<(int)strlen(string);i++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[i] = string[i] ;
   }
  temp[i] = (char) NULL ;
  id = atoi ( temp ) ;
 
  /* om inte aktuellt inl�gg s� skall inl�gget skrivas ned     */
  if ( id !=  title.id )
    fputs ( string, group  ) ;
  else
    break ;
 
}

  /* skriv ned �ndrad post */

fprintf( group, "%d\t%d\t%d\t%d\t%d\t", title.id, title.subject, 
        -1, title.owner, title.indent) ;
fprintf( group, "%s\t%d\t%d\t%s\t%s\t%s\t%s\n", title.timestamp, title.type,
         title.group, title.title, title.from, title.author, title.company ) ;

  /* skriver ned efterf�ljande poster */

while ( fgets ( string, 512, tmp ) != NULL )
  fputs ( string, group  ) ;

fclose ( tmp ) ;
fclose ( group ) ;
                     
return ( TRUE ) ;

} /* end of updateTitle*/


/******************************************************************

 Tar bort en befintlig rubrik om ett objekt i en fil 

 Innan funktionen anropas skall kategori/�mne vara l�st via funktionen
 lockSubject och efter�t skall uppl�sning g�ras via unlockSubject 

 Kategori eller �mne kan inte �ndras.

*******************************************************************/

int deleteTitle ( struct _title title ) 

{

char filename[128] ;    /* filnamn f�r fil som inneh�ller ett inl�gg */
char tmpname[128] ;
char string[512] ;      /* st�ng f�r l�sning till och fr�n textfil */

int id ;

FILE *tmp, *group ;

if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/news-%d-%d.dat", DB_PATH, title.category, title.subject ) ;
else
  sprintf( filename, "%s/news-%d.dat", DB_PATH, title.subject ) ;

sprintf( tmpname, "%s/news-%d.tmp", DB_PATH, title.subject ) ;

if (( group = fopen ( filename, "r" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

if (( tmp = fopen ( tmpname, "w" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

/* skapa en kopia */

while ( fgets ( string, 512, group ) != NULL )
      fputs ( string, tmp  ) ;  

fclose ( tmp ) ;
fclose ( group ) ;

/* F�r �ver alla poster utom den som skall raderas till orginalfilen */

if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/news-%d-%d.dat", DB_PATH, title.category, title.subject ) ;
else
  sprintf( filename, "%s/news-%d.dat", DB_PATH, title.subject ) ;

sprintf( tmpname, "%s/news-%d.tmp", DB_PATH, title.subject ) ;

if (( group = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

if (( tmp = fopen ( tmpname, "r" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

 /* om inte r�tt id s� spara ned posten */

while ( fgets ( string, 512, tmp ) != NULL )
 {      
   id = atoi ( string ) ;   /* artikelnummer som f�rsta f�lt innan f�ltavskiljare */
   if ( id != title.id )
     fputs ( string, group  ) ;  
 }

return ( TRUE ) ;

} /* end of deleteTitle */




/***************************************************************************************************************

 radera ett �mne 

*****************************************************************************************************************/



deleteSubject ( struct _subject *subject )

{

char filename[128] ;    /* filnamn f�r fil som inneh�ller ett inl�gg */
char tmpname[128] ; 
char string[512];       /* inl�st str�ng fr�n fil */
 int id ;                       /* id p� aktuell post */

FILE *fp, *tmp, *lock ;

    /* s� l�nge denna filen �r �ppna kan inga andra nyhetsgrupper �ndras/skapas */


if ( strcmp( EDITION, "Limited"))
  sprintf ( filename, "%s/group-%d.lck", DB_PATH , subject->category ) ;
else
  sprintf ( filename, "%s/group.lck" , DB_PATH ) ;

if (( lock = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("Nyhetsgruppen kan ej sparas. Fel vid postl�sning") ;
    return ( FALSE ) ;
   }

if ( strcmp ( EDITION, "Limited"))
  sprintf ( filename, "%s/group-%d.dat", DB_PATH, subject->category ) ;
else
  sprintf ( filename, "%s/group.dat" , DB_PATH  ) ;

 /* skapa en kopia av filen med inl�gg */

if (( fp = fopen ( filename, "r" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej l�sas.") ;
    return ( -1 ) ;
   }


if ( strcmp( EDITION, "Limited"))
  sprintf( tmpname, "%s/group-%d-%d.tmp", DB_PATH, subject->category, subject->id ) ;
else
 sprintf( tmpname, "%s/group-%d.tmp", DB_PATH, subject->category ) ;
  

if (( tmp = fopen ( tmpname, "w")) == NULL )
 {
    errorHTML ("Kopia  kan ej skrivas.") ;
    return ( -1 ) ;
   }

while ( fgets ( string, 512, fp ) != NULL )
  fprintf ( tmp, "%s", string ) ;

fclose ( fp ) ;
fclose ( tmp ) ;

  /* spara ned id, namn och anm�rkning f�rst i en fil */        

if (( fp = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

if (( tmp = fopen ( tmpname, "r")) == NULL )
 {
    errorHTML ("Kopia  kan ej l�sas.") ;
    return ( -1 ) ;
   }

  /* f�r �ver tidigare inl�gg */

while ( fgets ( string, 512, tmp ) != NULL )
  {

   id = atoi ( string ) ;   /* artikelnummer som f�rsta f�lt innan f�ltavskiljare */
   if ( id != subject->id )
     fputs ( string, fp  ) ;  
    
  }
    /* st�ng filen f�r rubriker */

fclose (  tmp ) ;
fclose ( fp ) ;
    
  /* st�ng l�sningsfilen f�r id */    

unlockSubject ( lock ) ;
    
return ( TRUE ) ;    /*  nyhetsgruppen blev raderad  */

}   /* end of deleteSubject */


/*



*/

deleteMessage ( struct _message *message )

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

char filename[128] ;    /* filnamn f�r fil som inneh�ller ett inl�gg */
char tmpname[128] ;
char string[512] ;      /* st�ng f�r l�sning till och fr�n textfil */
int id ;

FILE  *tmp, *group, *lock ;


    /* s� l�nge denna filen �r �ppna kan inga andra inl�gg i gruppen g�ras */


if ((lock = lockSubject ( message->category, message->subject )) == NULL ) 
  return ( FALSE ) ;

if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/news-%d-%d.dat", DB_PATH, message->category, message->subject ) ;
else
  sprintf( filename, "%s/news-%d.dat", DB_PATH, message->subject ) ;

sprintf( tmpname, "%s/news-%d.tmp", DB_PATH, message->subject ) ;

if (( group = fopen ( filename, "r" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

if (( tmp = fopen ( tmpname, "w" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

/* skapa en kopia */

while ( fgets ( string, 512, group ) != NULL )
      fputs ( string, tmp  ) ;  

fclose ( tmp ) ;
fclose ( group ) ;

/* F�r �ver alla poster utom den som skall raderas till orginalfilen */

if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/news-%d-%d.dat", DB_PATH, message->category, message->subject ) ;
else
  sprintf( filename, "%s/news-%d.dat", DB_PATH, message->subject ) ;

sprintf( tmpname, "%s/news-%d.tmp", DB_PATH, message->subject ) ;

if (( group = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

if (( tmp = fopen ( tmpname, "r" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

 /* om inte r�tt id s� spara ned posten */

while ( fgets ( string, 512, tmp ) != NULL )
 {      
   id = atoi ( string ) ;   /* artikelnummer som f�rsta f�lt innan f�ltavskiljare */
   if ( id != message->id )
     fputs ( string, group  ) ;  
 }

  /* st�ng l�sningsfilen f�r id */    

unlockSubject ( lock ) ;

return ( TRUE ) ;
}


deleteArticle ( struct _article *article )

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

char filename[128] ;    /* filnamn f�r fil som inneh�ller ett inl�gg */
char tmpname[128] ;
char string[512] ;      /* st�ng f�r l�sning till och fr�n textfil */
int id ;

FILE  *tmp, *group, *lock ;


    /* s� l�nge denna filen �r �ppna kan inga andra inl�gg i gruppen g�ras */


if ((lock = lockSubject ( article->category, article->subject )) == NULL ) 
  return ( FALSE ) ;

if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/news-%d-%d.dat", DB_PATH, article->category, article->subject ) ;
else
  sprintf( filename, "%s/news-%d.dat", DB_PATH, article->subject ) ;

sprintf( tmpname, "%s/news-%d.tmp", DB_PATH, article->subject ) ;

if (( group = fopen ( filename, "r" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

if (( tmp = fopen ( tmpname, "w" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

/* skapa en kopia */

while ( fgets ( string, 512, group ) != NULL )
      fputs ( string, tmp  ) ;  

fclose ( tmp ) ;
fclose ( group ) ;

/* F�r �ver alla poster utom den som skall raderas till orginalfilen */

if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/news-%d-%d.dat", DB_PATH, article->category, article->subject ) ;
else
  sprintf( filename, "%s/news-%d.dat", DB_PATH, article->subject ) ;

sprintf( tmpname, "%s/news-%d.tmp", DB_PATH, article->subject ) ;

if (( group = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

if (( tmp = fopen ( tmpname, "r" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

 /* om inte r�tt id s� spara ned posten */

while ( fgets ( string, 512, tmp ) != NULL )
 {      
   id = atoi ( string ) ;   /* artikelnummer som f�rsta f�lt innan f�ltavskiljare */
   if ( id != article->id )
     fputs ( string, group  ) ;  
 }

  /* st�ng l�sningsfilen f�r id */    

unlockSubject ( lock ) ;

return ( TRUE ) ;
}

deletePublication ( struct _article *article )

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

char filename[128] ;    /* filnamn f�r fil som inneh�ller ett inl�gg */
char tmpname[128] ;
char string[512] ;      /* st�ng f�r l�sning till och fr�n textfil */
int id ;

FILE  *tmp, *group, *lock ;


    /* s� l�nge denna filen �r �ppna kan inga andra inl�gg i gruppen g�ras */


if ((lock = lockSubject ( article->category, article->subject )) == NULL ) 
  return ( FALSE ) ;

if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/news-%d-%d.dat", DB_PATH, article->category, article->subject ) ;
else
  sprintf( filename, "%s/news-%d.dat", DB_PATH, article->subject ) ;

sprintf( tmpname, "%s/news-%d.tmp", DB_PATH, article->subject ) ;

if (( group = fopen ( filename, "r" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

if (( tmp = fopen ( tmpname, "w" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

/* skapa en kopia */

while ( fgets ( string, 512, group ) != NULL )
      fputs ( string, tmp  ) ;  

fclose ( tmp ) ;
fclose ( group ) ;

/* F�r �ver alla poster utom den som skall raderas till orginalfilen */

if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/news-%d-%d.dat", DB_PATH, article->category, article->subject ) ;
else
  sprintf( filename, "%s/news-%d.dat", DB_PATH, article->subject ) ;

sprintf( tmpname, "%s/news-%d.tmp", DB_PATH, article->subject ) ;

if (( group = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

if (( tmp = fopen ( tmpname, "r" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

 /* om inte r�tt id s� spara ned posten */

while ( fgets ( string, 512, tmp ) != NULL )
 {      
   id = atoi ( string ) ;   /* artikelnummer som f�rsta f�lt innan f�ltavskiljare */
   if ( id != article->id )
     fputs ( string, group  ) ;  
 }

  /* st�ng l�sningsfilen f�r id */    

unlockSubject ( lock ) ;

return ( TRUE ) ;
}


deleteLink ( struct _link *link )

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

char filename[128] ;    /* filnamn f�r fil som inneh�ller ett inl�gg */
char tmpname[128] ;
char string[512] ;      /* st�ng f�r l�sning till och fr�n textfil */
int id ;

FILE  *tmp, *group, *lock ;


    /* s� l�nge denna filen �r �ppna kan inga andra inl�gg i gruppen g�ras */


if ((lock = lockSubject ( link->category, link->subject )) == NULL ) 
  return ( FALSE ) ;

if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/news-%d-%d.dat", DB_PATH, link->category, link->subject ) ;
else
  sprintf( filename, "%s/news-%d.dat", DB_PATH, link->subject ) ;

sprintf( tmpname, "%s/news-%d.tmp", DB_PATH, link->subject ) ;

if (( group = fopen ( filename, "r" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

if (( tmp = fopen ( tmpname, "w" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

/* skapa en kopia */

while ( fgets ( string, 512, group ) != NULL )
      fputs ( string, tmp  ) ;  

fclose ( tmp ) ;
fclose ( group ) ;

/* F�r �ver alla poster utom den som skall raderas till orginalfilen */

if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/news-%d-%d.dat", DB_PATH, link->category, link->subject ) ;
else
  sprintf( filename, "%s/news-%d.dat", DB_PATH, link->subject ) ;

sprintf( tmpname, "%s/news-%d.tmp", DB_PATH, link->subject ) ;

if (( group = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

if (( tmp = fopen ( tmpname, "r" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

 /* om inte r�tt id s� spara ned posten */

while ( fgets ( string, 512, tmp ) != NULL )
 {      
   id = atoi ( string ) ;   /* artikelnummer som f�rsta f�lt innan f�ltavskiljare */
   if ( id != link->id )
     fputs ( string, group  ) ;  
 }

  /* st�ng l�sningsfilen f�r id */    

unlockSubject ( lock ) ;

return ( TRUE ) ;
}


 /* skriv information om ett inl�gg till en textfil */
 
insertArticle ( struct _article *article )

  /*
    
    Skriver ett inl�gg till tv� olika filer

    news-?.dat som inneh�ller titel
    msg-?-?.dat som inneh�ller sj�lva inl�gget

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
struct _title title ;

FILE *fp, *lock ;

    /* s� l�nge denna filen �r �ppna kan inga andra inl�gg i gruppen g�ras */

if ((lock = lockSubject ( article->category, article->subject )) == NULL ) 
  return ( FALSE ) ;

    /* h�mta nummer f�r n�sta inl�gg */

article->id = GetNextTitleId ( article->category, article->subject ) ;

if ( article->id == ERROR ) 
  return ( FALSE ) ;
    
    /* spara ned id, rubrik, f�rfattare och timestamp i fil */        

title.id = article->id ;
title.category = article->category ;
title.subject = article->subject ;
title.owner = article->owner ;
title.protect = article->protect ;
title.group = article->group ;
title.indent = article->indent ;     
title.type = 2 ;                     /* inl�gg */
title.comment = article->comment ;   /* f�reg�ende inl�gg */

strcpy ( title.timestamp, article->timestamp ) ;
strcpy ( title.title, article->title ) ;
strcpy ( title.from, article->from ) ;
strcpy ( title.author, article->author ) ;
strcpy ( title.company, article->company ) ;

insertTitle ( title ) ;

    /* �ppna filen f�r skrivning av en artikel */


if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/message-%d-%d-%d.dat",DB_PATH,  article->category, article->subject, article->id  ) ;
else 
  sprintf( filename, "%s/message-%d-%d.dat", DB_PATH, article->subject, article->id ) ; 

if (( fp = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej skrivas.") ;
    return ( -1 ) ;
   }

  /* skriv inl�gg till fil fil */

fputs ( article->text, fp ) ;

fclose ( fp ) ;

  /* st�ng l�sningsfilen f�r id */    

unlockSubject ( lock ) ;

return ( TRUE ) ;

} /* end of insertArticle () */

/**********************************************************************/

updateArticle ( struct _article *article )

  /*
    
    Skriver ett inl�gg till tv� olika filer soreterade i omv�nd kronologisk ordning

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

char filename[128] ;    /* filnamn f�r fil som inneh�ller ett inl�gg */
struct _title title ;

FILE *fp, *lock ;

    /* s� l�nge denna filen �r �ppna kan inga andra inl�gg i gruppen g�ras */

if ((lock = lockSubject ( article->category, article->subject )) == NULL ) 
  return ( FALSE ) ;
    
    /* spara ned id, rubrik, f�rfattare och timestamp i fil */        

title.id = article->id ;
title.category = article->category ;
title.subject = article->subject ;
title.owner = article->owner ;
title.protect = article->protect ;
title.indent = 0 ;
title.type = 1 ;                              /* meddelande */

strcpy ( title.title, article->title ) ;
strcpy ( title.from, article->from ) ;
strcpy ( title.author, article->author ) ;
strcpy ( title.company, article->company ) ;
updateTitle ( title ) ;


    /* �ppna filen f�r skrivning av en artikel */


if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/message-%d-%d-%d.dat",DB_PATH,  article->category, article->subject, article->id  ) ;
else 
  sprintf( filename, "%s/message-%d-%d.dat", DB_PATH, article->subject, article->id ) ; 

if (( fp = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej skrivas.") ;
    return ( -1 ) ;
   }

  /* skriv inl�gg till fil fil */

fputs ( article->text, fp ) ;

fclose ( fp ) ;

  /* st�ng l�sningsfilen f�r id */    

unlockSubject ( lock ) ;

return ( TRUE ) ;


} /* end of insertMessage () */



/**********************************************************************/

 /* skriv information om ett inl�gg till en textfil */
 
insertPublication ( struct _article *article )

  /*
    
    Skriver ett inl�gg till tv� olika filer

    news-?.dat som inneh�ller titel
    msg-?-?.dat som inneh�ller sj�lva inl�gget

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
struct _title title ;

FILE *fp, *lock ;

    /* s� l�nge denna filen �r �ppna kan inga andra inl�gg i gruppen g�ras */

if ((lock = lockSubject ( article->category, article->subject )) == NULL ) 
  return ( FALSE ) ;

    /* h�mta nummer f�r n�sta inl�gg */

article->id = GetNextTitleId ( article->category, article->subject ) ;

if ( article->id == ERROR ) 
  return ( FALSE ) ;
    
    /* spara ned id, rubrik, f�rfattare och timestamp i fil */        

title.id = article->id ;
title.category = article->category ;
title.subject = article->subject ;
title.owner = article->owner ;
title.protect = article->protect ;
title.indent = article->indent ;     
title.type = 9 ;                     /* publikation */
title.comment = article->comment ;   /* f�reg�ende inl�gg */
title.group = article->group ; 

strcpy ( title.timestamp, article->timestamp ) ;
strcpy ( title.title, article->title ) ;
strcpy ( title.from, article->from ) ;
strcpy ( title.author, article->author ) ;
strcpy ( title.company, article->company ) ;

insertTitle ( title ) ;

    /* �ppna filen f�r skrivning av en artikel */


if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/message-%d-%d-%d.dat",DB_PATH,  article->category, article->subject, article->id  ) ;
else 
  sprintf( filename, "%s/message-%d-%d.dat", DB_PATH, article->subject, article->id ) ; 

if (( fp = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej skrivas.") ;
    return ( -1 ) ;
   }

  /* skriv inl�gg till fil fil */

fputs ( article->text, fp ) ;

fclose ( fp ) ;

  /* st�ng l�sningsfilen f�r id */    

unlockSubject ( lock ) ;

return ( TRUE ) ;

} /* end of insertPublication () */

/**********************************************************************/

updatePublication ( struct _article *article )

  /*
    
    Skriver en post till tv� olika filer varav den en �r sorterad i omv�nd kronologisk ordning

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

char filename[128] ;    /* filnamn f�r fil som inneh�ller ett inl�gg */
struct _title title ;

FILE *fp, *lock ;

    /* s� l�nge denna filen �r �ppna kan inga andra inl�gg i gruppen g�ras */

if ((lock = lockSubject ( article->category, article->subject )) == NULL ) 
  return ( FALSE ) ;
    
    /* spara ned id, rubrik, f�rfattare och timestamp i fil */        

title.id = article->id ;
title.category = article->category ;
title.subject = article->subject ;
title.owner = article->owner ;
title.protect = article->protect ;
title.indent = 0 ;
title.type = 1 ;                              /* meddelande */

strcpy ( title.title, article->title ) ;
strcpy ( title.from, article->from ) ;
strcpy ( title.author, article->author ) ;
strcpy ( title.company, article->company ) ;
updateTitle ( title ) ;


    /* �ppna filen f�r skrivning av en artikel */

  /* �ppna filen f�r skrivning av en artikel */


if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/message-%d-%d-%d.dat",DB_PATH,  article->category, article->subject, article->id  ) ;
else 
  sprintf( filename, "%s/message-%d-%d.dat", DB_PATH, article->subject, article->id ) ; 

if (( fp = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej skrivas.") ;
    return ( -1 ) ;
   }

  /* skriv inl�gg till fil fil */

fputs ( article->text, fp ) ;

fclose ( fp ) ;

  /* st�ng l�sningsfilen f�r id */    

unlockSubject ( lock ) ;

return ( TRUE ) ;


} /* end of updatePublication () */

updateMessage ( struct _message *message )

  /*
    
    Skriver ett inl�gg till tv� olika filer soreterade i omv�nd kronologisk ordning

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

char filename[128] ;    /* filnamn f�r fil som inneh�ller ett inl�gg */
struct _title title ;

FILE *fp, *lock ;

    /* s� l�nge denna filen �r �ppna kan inga andra inl�gg i gruppen g�ras */

if ((lock = lockSubject ( message->category, message->subject )) == NULL ) 
  return ( FALSE ) ;
    
    /* spara ned id, rubrik, f�rfattare och timestamp i fil */        

title.id = message->id ;
title.category = message->category ;
title.subject = message->subject ;
title.owner = message->owner ;
title.protect = message->protect ;
title.indent = 0 ;
title.type = 3 ;                              /* meddelande */

strcpy ( title.title, message->title ) ;
strcpy ( title.from, message->from ) ;
strcpy ( title.author, message->author ) ;
strcpy ( title.company, message->company ) ;
strcpy ( title.timestamp, message->timestamp ) ;

updateTitle ( title ) ;

    /* �ppna filen f�r skrivning av en artikel */


if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/message-%d-%d-%d.dat",DB_PATH,  message->category, message->subject, message->id  ) ;
else 
  sprintf( filename, "%s/message-%d-%d.dat", DB_PATH, message->subject, message->id ) ; 

if (( fp = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej skrivas.") ;
    return ( -1 ) ;
   }

  /* skriv inl�gg till fil fil */

fputs ( message->text, fp ) ;

fclose ( fp ) ;

  /* st�ng l�sningsfilen f�r id */    

unlockSubject ( lock ) ;

return ( TRUE ) ;


} /* end of updateMessage () */


saveMessage ( struct _message *message )

  /*
    
    Skriver ett inl�gg till tv� olika filer soreterade i omv�nd kronologisk ordning

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

char filename[128] ;    /* filnamn f�r fil som inneh�ller ett inl�gg */
struct _title title ;

FILE *fp, *lock ;

    /* s� l�nge denna filen �r �ppna kan inga andra inl�gg i gruppen g�ras */

if ((lock = lockSubject ( message->category, message->subject )) == NULL ) 
  return ( FALSE ) ;

    /* h�mta nummer f�r n�sta inl�gg */

message->id = GetNextTitleId ( message->category, message->subject ) ;

if ( message->id == ERROR ) 
  return ( FALSE ) ;
    
    /* spara ned id, rubrik, f�rfattare och timestamp i fil */        

title.id = message->id ;
title.comment = message->comment ;
title.category = message->category ;
title.subject = message->subject ;
title.owner = message->owner ;
title.protect = message->protect ;
title.group = message->group ;
title.indent = message->indent ;
title.type = 3 ;                              /* meddelande */
title.link[0] = (char) NULL ;

strcpy ( title.title, message->title ) ;
strcpy ( title.from, message->from ) ;
strcpy ( title.from, message->to ) ;
strcpy ( title.author, message->author ) ;
strcpy ( title.company, message->company ) ;

insertTitle ( title ) ;

    /* �ppna filen f�r skrivning av en artikel */

if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/message-%d-%d-%d.dat",DB_PATH,  message->category, message->subject, message->id  ) ;
else 
  sprintf( filename, "%s/message-%d-%d.dat", DB_PATH, message->subject, message->id ) ; 

if (( fp = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej skrivas.") ;
    return ( -1 ) ;
   }

  /* skriv inl�gg till fil fil */

fputs ( message->text, fp ) ;

fclose ( fp ) ;

  /* st�ng l�sningsfilen f�r id */    

unlockSubject ( lock ) ;

return ( TRUE ) ;


} /* end of saveMessage () */

/**********************************************************************/

insertLink ( struct _link *link )

  /*
    
    Skriver en l�nk till en fil sorterade i omv�nd kronologisk ordning

    news-?.dat som inneh�ller rubrik
   
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

if ((lock = lockSubject ( link->category, link->subject )) == NULL ) 
  return ( FALSE ) ;

    /* h�mta nummer f�r n�sta inl�gg */

link->id = GetNextTitleId ( link->category, link->subject ) ;

if ( link->id == ERROR ) 
  return ( FALSE ) ;
    
    /* spara ned id, rubrik, f�rfattare och timestamp i fil */        

title.id = link->id ;
title.category = link->category ;
title.subject = link->subject ;
title.owner = link->owner ;
title.protect = link->protect ;
title.indent = 0 ;
title.type = 1 ;                              /* l�nk */

strcpy ( title.title, link->title ) ;
strcpy ( title.link, link->href ) ;
strcpy ( title.from, link->from ) ;
strcpy ( title.author, link->author ) ;
strcpy ( title.company, link->company ) ;

insertTitle ( title ) ;


    /* �ppna filen f�r skrivning av en artikel */


if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/message-%d-%d-%d.dat",DB_PATH,  link->category, link->subject, link->id  ) ;
else 
  sprintf( filename, "%s/message-%d-%d.dat", DB_PATH, link->subject, link->id ) ; 

if (( fp = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej skrivas.") ;
    return ( -1 ) ;
   }

  /* skriv inl�gg till fil fil */

fputs ( link->href, fp ) ;

fclose ( fp ) ;

  /* st�ng l�sningsfilen f�r id */    

unlockSubject ( lock ) ;

                     
return ( TRUE ) ; 

} /* end of insertLink () */


updateLink ( struct _link *link )

  /*
    
    Skriver ett inl�gg till tv� olika filer soreterade i omv�nd kronologisk ordning

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

char filename[128] ;    /* filnamn f�r fil som inneh�ller ett inl�gg */
struct _title title ;

FILE *fp, *lock ;

    /* s� l�nge denna filen �r �ppna kan inga andra inl�gg i gruppen g�ras */

if ((lock = lockSubject ( link->category, link->subject )) == NULL ) 
  return ( FALSE ) ;
    
    /* spara ned id, rubrik, f�rfattare och timestamp i fil */        

title.id = link->id ;
title.category = link->category ;
title.subject = link->subject ;
title.owner = link->owner ;
title.protect = link->protect ;
title.indent = 0 ;
title.type = 1 ;                              /* meddelande */

strcpy ( title.title, link->title ) ;
strcpy ( title.link, link->href ) ;
strcpy ( title.timestamp, link->timestamp ) ;
strcpy ( title.from, link->from ) ;
strcpy ( title.author, link->author ) ;
strcpy ( title.company, link->company ) ;

updateTitle ( title ) ;

    /* �ppna filen f�r skrivning av en artikel */


if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/message-%d-%d-%d.dat",DB_PATH,  link->category, link->subject, link->id  ) ;
else 
  sprintf( filename, "%s/message-%d-%d.dat", DB_PATH, link->subject, link->id ) ; 

if (( fp = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej skrivas.") ;
    return ( -1 ) ;
   }

  /* skriv inl�gg till fil fil */

fputs ( link->href, fp ) ;

fclose ( fp ) ;

  /* st�ng l�sningsfilen f�r id */    

unlockSubject ( lock ) ;

return ( TRUE ) ;


} /* end of updateLink () */




/********************************************************************/

insertCategory ( struct _category *category )


  /**

    Funktionen sparar ned posten f�r en categori
    och skapar nya filer f�r r�knare  

    category..dat - inneh�ller alla nyhetsgrupper
    category.id  - inneh�ller antal nyhetsgrupper
    category.lck - l�sningsfil
    
  */
{

char filename[128] ;    /* filnamn f�r fil som inneh�ller ett inl�gg */
char string[512];       /* inl�st str�ng fr�n fil */

FILE *fp, *log, *lock ;


    /* s� l�nge denna filen �r �ppna kan inga andra nyhetsgrupper skapas */

sprintf ( filename, "%s/category.lck" , DB_PATH ) ;

if (( lock = fopen ( filename, "w" )) == NULL )
   {
    errorHTML(" Kategori kan ej sparas. Fel vid postl�sning") ;
    return ( FALSE ) ;
   }

sprintf ( filename, "%s/category.id", DB_PATH ) ;

    /* h�mta nummer f�r n�sta kategori */

if (( log = fopen ( filename, "r" )) == NULL )
   {
    errorHTML ("Kategori kan ej sparas. Fel vid l�sning av id") ;
    return ( -1 ) ;
   }

if ( fgets ( string, 512, log ) != NULL )          /* l�s f�rsta fr�n fil som inneh�ller l�pnummer */
 {
  category->id = atoi ( string ) ;    /* senaste l�pnummer */
  category->id ++ ;                   /* �ka tal med ett */ 
 }
 
fclose ( log ) ;

    

    /* �ppna id filen och spara ned nummer */

if (( log = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("Kategori kan ej sparas. Fel vid l�sning av id") ;
    return ( -1 ) ;
   }

fprintf ( log, "%d", category->id ) ;
    
    /* st�ng filen f�r id */
                     
fclose ( log ) ;

sprintf ( filename, "%s/category.dat" , DB_PATH  ) ;

  /* spara ned id, namn och anm�rkning sist i en fil */        

if (( fp = fopen ( filename, "a" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

fprintf( fp, "%d\t%d\t%d\t%d\t%s\t%s\t%s\n", 
         category->id, category->owner, category->protect, 0,
         category->timestamp, 
         category->name, category->remark ) ;
                 
    /* st�ng filen f�r rubriker */

fclose ( fp ) ;

    /* st�ng l�sningsfilen f�r id */

fclose ( lock ) ;
    
    /* skapa en fil som inneh�ller antal inl�gg i den nya nyhetsgruppen */
    /* �ppna filen och spara ned nummer */
    
sprintf( filename, "%s/group-%d.id", DB_PATH, category->id ) ;

if (( log = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("�mne f�r kategori kan ej sparas. Fel vid skrivning av id") ;
    return ( -1 ) ;
   }

fprintf ( log, "%d", 0 ) ;

sprintf( filename, "%s/group-%d.dat", DB_PATH, category->id ) ;

if (( fp = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("�mne f�r kategory kan ej sparas. Fel vid skrivning av tom post") ;
    return ( -1 ) ;
   }

fclose ( log ) ;
fclose ( fp ) ;

return ( TRUE ) ;

} /**/



updateCategory ( struct _category *category )


  /**

    Funktionen sparar ned posten f�r en categori
    och skapar nya filer f�r r�knare  

    category..dat - inneh�ller alla nyhetsgrupper
    category.id  - inneh�ller antal nyhetsgrupper
    category.lck - l�sningsfil
    
  */
{

char tmpname[1288] ;
char filename[128] ;    /* filnamn f�r fil som inneh�ller ett inl�gg */
char string[512];       /* inl�st str�ng fr�n fil */

FILE *fp, *tmp, *lock ;


    /* s� l�nge denna filen �r �ppna kan inga andra nyhetsgrupper skapas */

sprintf ( filename, "%s/category.lck" , DB_PATH ) ;

if (( lock = fopen ( filename, "w" )) == NULL )
   {
    errorHTML(" Kategori kan ej sparas. Fel vid postl�sning") ;
    return ( FALSE ) ;
   }

    /* kopiera befintlig fil med kategorier */


    /* spara ned id, rubrik, f�rfattare och timestamp i fil */        

if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/category.dat", DB_PATH ) ;


sprintf( tmpname, "%s/catagory.tmp" ) ;

if (( fp= fopen ( filename, "r" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

if (( tmp = fopen ( tmpname, "w" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

/* skapa en kopia */

while ( fgets ( string, 512, fp ) != NULL )
{
    fputs ( string, tmp  ) ;
}

fclose ( tmp ) ;
fclose ( fp ) ;

/* */

sprintf ( filename, "%s/category.dat" , DB_PATH  ) ;
sprintf ( tmpname, "%s/category.tmp" , DB_PATH  ) ;

  

  /* spara ned id, namn och anm�rkning i en fil */        


if (( fp = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

if (( tmp = fopen ( tmpname, "r" )) == NULL )
   {
    errorHTML ("Tempor�r fil kan ej l�sas.") ;
    return ( -1 ) ;
   }

 /* kopiera �ver alla f�reg�ende poster */



 /* uppdatera posten */

fprintf( fp, "%d\t%d\t%d\t%d\t%s\t%s\t%s\n", 
         category->id, category->owner, category->protect, 0,
         category->timestamp, 
         category->name, category->remark ) ;
           

 /* kopiera �ver resterande poster */

      
 /* st�ng filen f�r rubriker */

fclose ( fp ) ;
fclose ( tmp ) ;


    /* st�ng l�sningsfilen f�r id */

fclose ( lock ) ;
return ( TRUE ) ;

} /* end of updateCategory */


deleteCategory ( struct _category *category )


  /**

    Funktionen sparar ned posten f�r en categori
    och skapar nya filer f�r r�knare  

    category..dat - inneh�ller alla nyhetsgrupper
    category.id  - inneh�ller antal nyhetsgrupper
    category.lck - l�sningsfil
    
  */
{

char filename[128] ;    /* filnamn f�r fil som inneh�ller ett inl�gg */
char string[512];       /* inl�st str�ng fr�n fil */

FILE *fp, *log, *lock ;


    /* s� l�nge denna filen �r �ppna kan inga andra nyhetsgrupper skapas */

sprintf ( filename, "%s/category.lck" , DB_PATH ) ;

if (( lock = fopen ( filename, "w" )) == NULL )
   {
    errorHTML(" Kategori kan ej sparas. Fel vid postl�sning") ;
    return ( FALSE ) ;
   }

sprintf ( filename, "%s/category.id", DB_PATH ) ;

    /* h�mta nummer f�r n�sta kategori */

if (( log = fopen ( filename, "r" )) == NULL )
   {
    errorHTML ("Kategori kan ej sparas. Fel vid l�sning av id") ;
    return ( -1 ) ;
   }

if ( fgets ( string, 512, log ) != NULL )          /* l�s f�rsta fr�n fil som inneh�ller l�pnummer */
 {
  category->id = atoi ( string ) ;    /* senaste l�pnummer */
  category->id ++ ;                   /* �ka tal med ett */ 
 }
 
fclose ( log ) ;

    

    /* �ppna id filen och spara ned nummer */

if (( log = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("Kategori kan ej sparas. Fel vid l�sning av id") ;
    return ( -1 ) ;
   }

fprintf ( log, "%d", category->id ) ;
    
    /* st�ng filen f�r id */
                     
fclose ( log ) ;

sprintf ( filename, "%s/category.dat" , DB_PATH  ) ;

  /* spara ned id, namn och anm�rkning sist i en fil */        

if (( fp = fopen ( filename, "a" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

fprintf( fp, "%d\t%d\t%d\t%d\t%s\t%s\t%s\n", 
         category->id, category->owner, category->protect, 0,
         category->timestamp, 
         category->name, category->remark ) ;
                 
    /* st�ng filen f�r rubriker */

fclose ( fp ) ;

    /* st�ng l�sningsfilen f�r id */

fclose ( lock ) ;
    
    /* skapa en fil som inneh�ller antal inl�gg i den nya nyhetsgruppen */
    /* �ppna filen och spara ned nummer */
    
sprintf( filename, "%s/group-%d.id", DB_PATH, category->id ) ;

if (( log = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("�mne f�r kategori kan ej sparas. Fel vid skrivning av id") ;
    return ( -1 ) ;
   }

fprintf ( log, "%d", 0 ) ;

sprintf( filename, "%s/group-%d.dat", DB_PATH, category->id ) ;

if (( fp = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("�mne f�r kategory kan ej sparas. Fel vid skrivning av tom post") ;
    return ( -1 ) ;
   }

fclose ( log ) ;
fclose ( fp ) ;

return ( TRUE ) ;

} /**/





 /* sparar ett �mne */
 
insertSubject ( struct _subject *subject )

  /**

    Funktionen sparar ned posten f�r en nyhetsgrupp 
    och skapar nya filer f�r r�knare  

    group.dat - inneh�ller alla nyhetsgrupper
    group.id  - inneh�ller antal nyhetsgrupper
    group.lck - l�sningsfil
    
  */
{

char filename[128] ;    /* filnamn f�r fil som inneh�ller ett inl�gg */
char tmpname[128] ; 
char string[512];       /* inl�st str�ng fr�n fil */

FILE *fp, *tmp, *log, *lock ;

    /* s� l�nge denna filen �r �ppna kan inga andra nyhetsgrupper skapas */


if ( strcmp( EDITION, "Limited"))
  sprintf ( filename, "%s/group-%d.lck", DB_PATH , subject->category ) ;
else
  sprintf ( filename, "%s/group.lck" , DB_PATH ) ;

if (( lock = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("Nyhetsgruppen kan ej sparas. Fel vid postl�sning") ;
    return ( FALSE ) ;
   }

    /* h�mta nummer f�r n�sta nyhetsgrupp */


if ( strcmp( EDITION, "Limited"))
 sprintf ( filename, "%s/group-%d.id", DB_PATH, subject->category ) ;
else
  sprintf ( filename, "%s/group.id", DB_PATH ) ;

if (( log = fopen ( filename, "r" )) == NULL )
   {
    errorHTML ("Nyhetsgruppen kan ej sparas. Fel vid l�sning av id") ;
    return ( -1 ) ;
   }

if ( fgets ( string, 512, log ) != NULL )          /* l�s f�rsta fr�n fil som inneh�ller l�pnummer */
 {
  subject->id = atoi ( string ) ;    /* senaste l�pnummer */
  subject->id ++ ;                   /* �ka tal med ett */ 
 }
 
fclose ( log ) ;

   /* �ppna id filen och spara ned nummer */

if (( log = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas. Fel vid l�sning av id") ;
    return ( -1 ) ;
   }

fprintf ( log, "%d", subject->id ) ;
     
    /* st�ng filen f�r id */
                     
fclose ( log ) ;

if ( strcmp ( EDITION, "Limited"))
  sprintf ( filename, "%s/group-%d.dat", DB_PATH, subject->category ) ;
else
  sprintf ( filename, "%s/group.dat" , DB_PATH  ) ;

 /* skapa en kopia av filen med inl�gg */

if (( fp = fopen ( filename, "r" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej l�sas.") ;
    return ( -1 ) ;
   }


if ( strcmp( EDITION, "Limited"))
  sprintf( tmpname, "%s/group-%d-%d.tmp", DB_PATH, subject->category, subject->id ) ;
else
 sprintf( tmpname, "%s/group-%d.tmp", DB_PATH, subject->category ) ;
  

if (( tmp = fopen ( tmpname, "w")) == NULL )
 {
    errorHTML ("Kopia  kan ej skrivas.") ;
    return ( -1 ) ;
   }

while ( fgets ( string, 512, fp ) != NULL )
  fprintf ( tmp, "%s", string ) ;

fclose ( fp ) ;
fclose ( tmp ) ;

  /* spara ned id, namn och anm�rkning f�rst i en fil */        

if (( fp = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("Inl�gget kan ej sparas.") ;
    return ( -1 ) ;
   }

if (( tmp = fopen ( tmpname, "r")) == NULL )
 {
    errorHTML ("Kopia  kan ej l�sas.") ;
    return ( -1 ) ;
   }

fprintf( fp, "%d\t%d\t%d\t%d\t%s\t%s\t%s\n", 
         subject->id, subject->owner, subject->protect, 
         subject->article, subject->timestamp, 
         subject->name, subject->remark ) ;
                 
 
  /* f�r �ver tidigare inl�gg */

while ( fgets ( string, 512, tmp ) != NULL )
  fprintf ( fp, "%s", string ) ;

    /* st�ng filen f�r rubriker */

fclose (  tmp ) ;
fclose ( fp ) ;
    
     /* st�ng l�sningsfilen f�r id */

fclose ( lock ) ;

    
    /* skapa en fil som inneh�ller antal inl�gg i den nya nyhetsgruppen */
    /* �ppna filen och spara ned nummer */


if ( strcmp ( EDITION, "Limited"))
  sprintf( filename, "%s/news-%d-%d.id", DB_PATH, subject->category, subject->id ) ;
else
  sprintf( filename, "%s/news-%d.id", DB_PATH, subject->id ) ;

if (( log = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("Nyhetsgruppen kan ej sparas. Fel vid skrivning av id") ;
    return ( -1 ) ;
   }

fprintf ( log, "%d", 0 ) ;



if ( strcmp ( EDITION, "Limited"))
  sprintf( filename, "%s/news-%d-%d.dat", DB_PATH, subject->category, subject->id ) ;
else
  sprintf( filename, "%s/news-%d.dat", DB_PATH, subject->id ) ;

if (( fp = fopen ( filename, "w" )) == NULL )
   {
    errorHTML ("Nyhetsgruppen kan ej sparas. Fel vid skrivning av id") ;
    return ( -1 ) ;
   }
    /* st�ng filen f�r id */

fclose ( fp ) ;
fclose ( log ) ;

return ( TRUE ) ;    /*  nyhetsgruppen blev sparad */

}

 /* l�s aktuella nyhetsgrupper */

getNextmessageId ( int category, int id ) 

{

char filename[128] ;
char string[255] ;

FILE *log ;


if ( strcmp ( EDITION, "Limited"))
  sprintf( filename, "%s/news-%d-%d", DB_PATH, category, id ) ;
else
  sprintf( filename, "%s/news-%d.id", DB_PATH, id ) ;

if (( log = fopen ( filename, "r" )) == NULL )
   {
    return ( -1 ) ;
   }
   
if ( fgets ( string, 512, log ) != NULL )  /* l�s tal fr�n str�ng */
  id = atoi ( string ) ;
      
    /* st�ng filen f�r n�sta nummer */

fclose ( log ) ;
    
return ( id ) ;    

}
 
selectSubject ( struct _subject *subject, char *show, char *sort, char *cursor )

   /* subject - l�nkad lista med aktuella nyhetsgrupper
      show      - ALL, visa alla inl�gg
      sort      - DATE, sortera efter datum
      cursor    - NULL
         
   */

{

int i,j ;               /* r�knare f�r str�ngar */
int counter ;           /* antal poster */
char filename[128] ;    /* filnamn f�r fil som inneh�ller nyhetsgrupper */
char string[512];       /* rad f�r namn */
char temp[20] ;         /* omvandling string -> heltal */

struct _subject *first ; /* f�rsta inl�gg */
struct _subject *ptr ;  /* n�sta inl�gg */  

FILE *fp, *lock ;

filename[0] = (char) NULL ; 

   /* fins det en eller flera katagorier */

if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/group-%d.lck", DB_PATH, subject->category) ; 
else
  sprintf( filename, "%s/group.lck", DB_PATH ) ; 

if (( lock = fopen ( filename, "w" )) == NULL )
   {
    sprintf ( string, "Nyhetsgrupper %s kan ej l�sas.", filename ) ;
    errorHTML ( string ) ;
    return ( -1 ) ;
   }

    /* �ppna filen f�r l�sning */

if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/group-%d.dat", DB_PATH, subject->category ) ; 
else
  sprintf( filename, "%s/group.dat" , DB_PATH ) ; 

if (( fp = fopen ( filename, "r" )) == NULL )
   {
    sprintf ( string, "Nyhetsgrupper %s kan ej l�sas.", filename ) ;
    errorHTML ( string ) ;
    return ( -1 ) ;
   }

  
   /* adressen till f�rsta posten blir startadress f�r en l�nkad lista */
   
counter = 0 ;
subject->next = NULL ;
first = subject ;

  /* l�s rubriker fr�n fil */

while ( fgets ( string, 512, fp ) != NULL )
 {
     /* skapa minne f�r en ny nyhetsgrupp */
     /* f�rsta posten finns redan allokerad */  

   if ( counter > 0 )
    {  
     ptr = malloc ( sizeof ( struct _subject )) ;  /* adress till n�sta post */
     ptr->next = NULL ;                             /* efterf�ljande adress saknas */
     subject->next = ptr ;                        /* l�gg till adress till befintlig post */
     subject = ptr ;                              /* ny adress f�r post */  
    } 
  
  counter ++ ;                      /* antal poster */
    
   /* plocka fram id f�r varje nyhetsgrupp */ 
   
  for (i=0;i<(int)strlen(string);i++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[i] = string[i] ;
   }
  temp[i] = (char) NULL ;
  subject->id = atoi ( temp ) ;
  i++ ;
  
    /* owner */
  
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;
  subject->owner = atoi ( temp ) ;
  i++ ;      
   
    /* protect */
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;
  subject->protect = atoi ( temp ) ;
  i++ ;      
   
   /* senaste lagrade artikel*/
   
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;
  subject->article = atoi ( temp ) ;
  i++ ;      
  
   /* datumm�rkning, YYYY-MM-DD hh:mm:ss */
   
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    subject->timestamp[j] = string[i] ;
   }
  subject->timestamp[j] = (char) NULL ;
  i++ ;      
  
   /* plocka fram namn f�r varje nyhetsgrupp */

  for (j=0;i<(int)strlen(string);i++,j++)
   {
    if ( string[i] == '\t' )
      break ;      
    subject->name[j] = string[i] ;
   }

   subject->name[j] = (char) NULL ;
   i++ ;     

   /* plocka fram anm�rkning f�r varje nyhetsgrupp */
 
   for (j=0;i<(int)strlen(string);i++,j++)
    {
    if ( string[i] == '\t' )
      break ;      
    subject->remark[j] = string[i] ;
    }
   subject->remark[j] = (char) NULL ;
   i++ ;     
  }

fclose ( fp ) ;
fclose ( lock ) ;
return ( counter ) ;  

}


selectCategory ( struct _category *category, char *show, char *sort, char *cursor )

   /* category - l�nkad lista med aktuella katagorier
      show      - ALL, visa alla inl�gg
      sort      - DATE, sortera efter datum
      cursor    - NULL
         
   */

{

int i,j ;               /* r�knare f�r str�ngar */
int counter ;           /* antal poster */
char filename[128] ;    /* filnamn f�r fil som inneh�ller nyhetsgrupper */
char string[512];       /* rad f�r namn */
char temp[20] ;         /* omvandling string -> heltal */

struct _category *first ; /* f�rsta inl�gg */
struct _category *ptr ;  /* n�sta inl�gg */  

FILE *fp, *lock ;

filename[0] = (char) NULL ; 

   /* fins det en eller flera katagorier */
sprintf( filename, "%s/category.lck", DB_PATH  ) ; 

if (( lock = fopen ( filename, "w" )) == NULL )
   {
    sprintf ( string, "Kategorier %s kan ej l�sas.", filename ) ;
    errorHTML ( string ) ;
    return ( -1 ) ;
   }

    /* �ppna filen f�r l�sning */

sprintf( filename, "%s/category.dat", DB_PATH  ) ; 

if (( fp = fopen ( filename, "r" )) == NULL )
   {
    sprintf ( string, "Kategorier %s kan ej l�sas.", filename ) ;
    errorHTML ( string ) ;
    return ( -1 ) ;
   }

  
   /* adressen till f�rsta posten blir startadress f�r en l�nkad lista */
   
counter = 0 ;
category->next = NULL ;
first = category ;

  /* l�s rubriker fr�n fil */

while ( fgets ( string, 512, fp ) != NULL )
 {
     /* skapa minne f�r en ny nyhetsgrupp */
     /* f�rsta posten finns redan allokerad */  

   if ( counter > 0 )
    {  
     ptr = malloc ( sizeof ( struct _category )) ;  /* adress till n�sta post */
     ptr->next = NULL ;                             /* efterf�ljande adress saknas */
     category->next = ptr ;                        /* l�gg till adress till befintlig post */
     category = ptr ;                              /* ny adress f�r post */  
    } 
  
  counter ++ ;                      /* antal poster */
    
   /* plocka fram id f�r varje kategori */ 
   
  for (i=0;i<(int)strlen(string);i++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[i] = string[i] ;
   }
  temp[i] = (char) NULL ;
  category->id = atoi ( temp ) ;
  i++ ;
  
    /* owner */
  
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;
  category->owner = atoi ( temp ) ;
  i++ ;      
   
    /* protect */
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;
  category->protect = atoi ( temp ) ;
  i++ ;      
   
   /* senaste lagrade artikel*/
   
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;
  i++ ;      
  
   /* datumm�rkning, YYYY-MM-DD hh:mm:ss */
   
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    category->timestamp[j] = string[i] ;
   }
  category->timestamp[j] = (char) NULL ;
  i++ ;      
  
   /* plocka fram namn f�r varje nyhetsgrupp */

  for (j=0;i<(int)strlen(string);i++,j++)
   {
    if ( string[i] == '\t' )
      break ;      
    category->name[j] = string[i] ;
   }

   category->name[j] = (char) NULL ;
   i++ ;     

   /* plocka fram anm�rkning f�r varje nyhetsgrupp */
 
   for (j=0;i<(int)strlen(string);i++,j++)
    {
    if ( string[i] == '\t' )
      break ;      
    category->remark[j] = string[i] ;
    }
   category->remark[j] = (char) NULL ;
   i++ ;     
  }

fclose ( fp ) ;
fclose ( lock ) ;
return ( counter ) ;  

}

   /*  */

readTitle ( char *string, struct _title *cur_title ) 

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
  cur_title->id = atoi ( temp ) ;
  i++ ;
 
  /* aktuellt �mne */

  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;
  cur_title->subject = atoi ( temp ) ;
  i++ ;
  
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;
  cur_title->protect = atoi ( temp ) ;
  i++ ;                         
  
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;
  cur_title->owner = atoi ( temp ) ;
  i++ ;
  
     /* niv� p� indrag */
     
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;
  cur_title->indent = atoi ( temp ) ;
  i++ ;

    /* plocka fram tidst�mpling f�r varje rubrik */

   for (j=0;i<(int)strlen(string);i++,j++)
    {
    if ( string[i] == '\t' )
      break ;      
    cur_title->timestamp[j] = string[i] ;
    }
   cur_title->timestamp[j] = (char) NULL ;
   i++ ;        
   
     /* dokumenttyp  1 - HTML l�nk, 2 - Article, 3 - Message, 4 - Dokumentl�nk */
     
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;
  cur_title->type = atoi ( temp ) ;
  i++ ;
                              
    /* group */
      
  for (j=0;i<(int)strlen(string);i++,j++)
    {
    if ( string[i] == '\t' )
      break ;      
    temp[j] = string[i] ;
    }
   temp[j] = (char) NULL ;
   cur_title->group = atoi ( temp ) ;
   i++ ;

   /* plocka fram titel f�r varje rubrik */

   for (j=0;i<(int)strlen(string);i++,j++)
    {
    if ( string[i] == '\t' )
      break ;      
    cur_title->title[j] = string[i] ;
    }
   cur_title->title[j] = (char) NULL ;
   i++ ;

    /* plocka fram e-post f�r varje rubrik */

   for (j=0;i<(int)strlen(string);i++,j++)
    {
    if ( string[i] == '\t' )
      break ;      
    cur_title->from[j] = string[i] ;
    }
   cur_title->from[j] = (char) NULL ;
   i++ ;        
           
    /* plocka fram namn f�r varje rubrik */

   for (j=0;i<(int)strlen(string);i++,j++)
    {
    if ( string[i] == '\t' )
      break ;      
    cur_title->author[j] = string[i] ;
    }
   cur_title->author[j] = (char) NULL ;
   i++ ;        

    /* hoppa �ver f�retag f�r varje rubrik */

   for (j=0;i<(int)strlen(string);i++,j++)
    {
    if ( string[i] == '\t' )
      break ;      
    }
   i++ ;        

   cur_title->company[0] = (char) NULL ;

   return ( TRUE ) ;

}  /* end of readTitle */ 


 /* Get title information about one object */

getTitle ( int category_id, int subject_id, int object_id, struct _title *title ) 


{

int counter ;           /* antal l�sta poster */


char filename[128] ;    /* filnamn f�r fil som inneh�ller ett inl�gg */
char string[512];       /* rad f�r rubrik */

               
FILE *fp, *lock ;

counter = TRUE ;  

  /* Om l�sfilen inte g�r att �ppna skall man inte forts�tta */

if ( strcmp( EDITION, "Limited"))
  sprintf( filename, "%s/news-%d-%d.lck", DB_PATH, category_id, subject_id ) ; 
else
  sprintf( filename, "%s/news-%d.lck", DB_PATH, subject_id ) ; 

title->category = category_id ;
title->subject = subject_id ;

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

counter = 0 ; 

while ( fgets ( string, 255, fp ) != NULL )
  {                                              

   /* plocka fram uppgifter f�r varje rubrik */

   readTitle ( string, title ) ;
   
   if ( object_id == title->id )
   {
    counter ++ ;
    break ;
   }  /* end of if */

 }  /* end of while */

fclose ( fp ) ;
fclose ( lock ) ;

return ( counter ) ;

} /* end of getTitle */




 
selectArticle ( int category_id, int subject_id, int object_id, struct _article *article )
 
 /* catagory_id, subject_id and object_id 
    anv�nds f�r att avg�ra vilken fil som skall l�sas 
    �vriga v�rden h�mtas fr�n fil 
    
    om filen saknas returnerar funktionen ERROR
    annars returneras TRUE
    
    kontroll av f�ltl�ngder g�rs. 
    Om inte inl�st f�lt f�r plats kapas efterf�ljande tecken.
    returv�rdet s�tts d� till FALSE
    
    */
    
{


char filename[128] ;    /* filnamn f�r fil som inneh�ller ett inl�gg */
char string[255];       /* rad i meddelande */
struct _title title ;

FILE *fp ;


if ( !getTitle ( category_id, subject_id, object_id, &title ))
{
  errorHTML ("Article not found\n") ;
  exit(0) ;
} 

article->category = title.category ;
article->subject = title.subject ;
article->id = title.id ;
article->owner = title.owner ;
article->protect = title.protect ;
article->group = title.group ;

strcpy ( article->title, title.title ) ;
strcpy ( article->author, title.author ) ;
strcpy ( article->from, title.from ) ;
strcpy ( article->timestamp, title.timestamp ) ;
strcpy ( article->company, title.company ) ;

    /* �ppna filen f�r l�sning */

if ( strcmp ( EDITION, "Limited"))
  sprintf( filename, "%s/message-%d-%d-%d.dat", DB_PATH, 
               category_id,  subject_id, object_id ) ; 
else
  sprintf( filename, "%s/message-%d-%d.dat", DB_PATH, article->subject, article->id ) ; 

if (( fp = fopen ( filename, "r" )) == NULL )
   {
    sprintf ( string, "Inl�gget %s kan ej l�sas.", filename ) ;
    errorHTML ( string) ;
    return ( -1 ) ;
   }

  /* l�s inl�gg fr�n fil */


  /* l�gg till �vriga textrader */

article->text[0] = (char) NULL ;

while ( fgets ( string, 255, fp ) != NULL )
  strcat( article->text, string ) ;

fclose ( fp ) ;

return ( TRUE ) ;

}  /* end of selectArticle */

selectPublication ( int category_id, int subject_id, int object_id, struct _article *article )
 
 /* catagory_id, subject_id and object_id 
    anv�nds f�r att avg�ra vilken fil som skall l�sas 
    �vriga v�rden h�mtas fr�n fil 
    
    om filen saknas returnerar funktionen ERROR
    annars returneras TRUE
    
    kontroll av f�ltl�ngder g�rs. 
    Om inte inl�st f�lt f�r plats kapas efterf�ljande tecken.
    returv�rdet s�tts d� till FALSE
    
    */
    
{


char filename[128] ;    /* filnamn f�r fil som inneh�ller ett inl�gg */
char string[255];       /* rad i meddelande */
struct _title title ;

FILE *fp ;


if ( !getTitle ( category_id, subject_id, object_id, &title ))
{
  errorHTML ("Publication not found\n") ;
  exit(0) ;
} 

article->category = title.category ;
article->subject = title.subject ;
article->id = title.id ;
article->owner = title.owner ;
article->protect = title.protect ;
article->comment = title.comment ;
article->group = title.group ;

strcpy ( article->title, title.title ) ;
strcpy ( article->author, title.author ) ;
strcpy ( article->from, title.from ) ;
strcpy ( article->timestamp, title.timestamp ) ;
strcpy ( article->company, title.company ) ;

    /* �ppna filen f�r l�sning */

if ( strcmp ( EDITION, "Limited"))
  sprintf( filename, "%s/message-%d-%d-%d.dat", DB_PATH, 
               category_id,  subject_id, object_id ) ; 
else
  sprintf( filename, "%s/message-%d-%d.dat", DB_PATH, article->subject, article->id ) ; 

if (( fp = fopen ( filename, "r" )) == NULL )
   {
    sprintf ( string, "Inl�gget %s kan ej l�sas.", filename ) ;
    errorHTML ( string) ;
    return ( -1 ) ;
   }

  /* l�s inl�gg fr�n fil */


  /* l�gg till �vriga textrader */

article->text[0] = (char) NULL ;

while ( fgets ( string, 255, fp ) != NULL )
  strcat( article->text, string ) ;

fclose ( fp ) ;

return ( TRUE ) ;

}  /* end of selectPublication */

selectMessage ( int category_id, int subject_id, int object_id, struct _message *message )
 
 /* catagory_id, subject_id and object_id 
    anv�nds f�r att avg�ra vilken fil som skall l�sas 
    �vriga v�rden h�mtas fr�n fil 
    
    om filen saknas returnerar funktionen ERROR
    annars returneras TRUE
    
    kontroll av f�ltl�ngder g�rs. 
    Om inte inl�st f�lt f�r plats kapas efterf�ljande tecken.
    returv�rdet s�tts d� till FALSE
    
    */
    
{


char filename[128] ;    /* filnamn f�r fil som inneh�ller ett inl�gg */
char string[255];       /* rad i meddelande */
struct _title title ;

FILE *fp ;


if ( !getTitle ( category_id, subject_id, object_id, &title ))
{
  errorHTML ("Message not found\n") ;
  exit(0) ;
} 

message->category = title.category ;
message->subject = title.subject ;
message->id = title.id ;
message->owner = title.owner ;
message->protect = title.protect ;

strcpy ( message->title, title.title ) ;
strcpy ( message->author, title.author ) ;
strcpy ( message->from, title.from ) ;
strcpy ( message->timestamp, title.timestamp ) ;
strcpy ( message->company, title.company ) ;

    /* �ppna filen f�r l�sning */

if ( strcmp ( EDITION, "Limited"))
  sprintf( filename, "%s/message-%d-%d-%d.dat", DB_PATH, 
               category_id,  subject_id, object_id ) ; 
else
  sprintf( filename, "%s/message-%d-%d.dat", DB_PATH, subject_id, object_id ) ; 

if (( fp = fopen ( filename, "r" )) == NULL )
   {
    sprintf ( string, "Message %s can not be read.", filename ) ;
    errorHTML ( string) ;
    return ( -1 ) ;
   }

  /* l�s inl�gg fr�n fil */


  /* l�gg till �vriga textrader */

message->text[0] = (char) NULL ;

while ( fgets ( string, 255, fp ) != NULL )
  strcat( message->text, string ) ;

fclose ( fp ) ;

return ( TRUE ) ;

}  /* end of selectMessage */

/* SelectLink */

selectLink ( int category_id, int subject_id, int object_id, struct _link *link )
 
 /* article.conferance, article.id, article.subject 
    anv�nds f�r att avg�ra vilken fil som skall l�sas 
    �vriga v�rden h�mtas fr�n fil 
    
    om filen saknas returnerar funktionen ERROR
    annars returneras TRUE
    
    kontroll av f�ltl�ngder g�rs. 
    Om inte inl�st f�lt f�r plats kapas efterf�ljande tecken.
    returv�rdet s�tts d� till FALSE
    
    */
    
{


char filename[128] ;    /* filnamn f�r fil som inneh�ller ett inl�gg */
char string[255];       /* rad i meddelande */

FILE *fp ;

    /* �ppna filen f�r l�sning */

if ( strcmp ( EDITION, "Limited"))
  sprintf( filename, "%s/message-%d-%d-%d.dat", DB_PATH, 
               link->category,  link->subject, link->id ) ; 
else
  sprintf( filename, "%s/message-%d-%d.dat", DB_PATH, link->subject, link->id ) ; 

if (( fp = fopen ( filename, "r" )) == NULL )
   {
    sprintf ( string, "L�nken %s kan ej l�sas.", filename ) ;
    errorHTML ( string) ;
    return ( -1 ) ;
   }

  /* l�s inl�gg fr�n fil */


  /* l�gg till �vriga textrader */

link->href[0] = (char) NULL ;

while ( fgets ( string, 255, fp ) != NULL )
  strcat( link->href, string ) ;


fclose ( fp ) ;

return ( TRUE ) ;

}  /* end of readLink */


/**/





  /* skicka grupp-post till alla prenumeranter */

void groupMail ( struct _article *article )

/*

 skickar ett meddelande till alla som prenumumererar p� �mnet
 
*/

{

int i,j ;
char temp[255] ;

int id ;
int category ;
int group ;
int notify ;
int subscribe ;

char string[255] ;
char command[255] ;
char title[64]; 
char from[24] ;
char to[64] ; 
char filename[128] ; 

FILE *fp ;

sprintf ( title, "Rubrik" ) ;
sprintf ( from, "Avs�ndare") ;
sprintf ( to, "Mottagare" ) ;

  /* �ppna filen f�r medlemmar */
  
sprintf( filename, "%s/subscribe.dat", DB_PATH ) ;

if (( fp = fopen ( filename, "r" )) == NULL )
 {
  printf("Filen %s kan ej �ppnas\n",  filename ) ;
  exit ( FALSE ) ;
 } 

while ( fgets ( string, 255, fp ) != NULL )
 {
 
  for (i=0;i<(int)strlen(string);i++ ) 
   {
    if ( string[i] == '\t' )
      break ;      
    temp[i] = string[i] ;
   }
  temp[i] = (char) NULL ;
  id = atoi ( temp) ;
  i++ ;        

 for (j=0;i<(int)strlen(string);i++,j++)
    {
    if ( string[i] == '\t' )
      break ;      
    to[j] = string[i] ;
    }
   to[j] = (char) NULL ;
   i++ ;     

  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;                 
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;
  category = atoi ( temp) ;
  i++ ;        

  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;                 
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;
  group = atoi ( temp) ;
  i++ ;        
  
  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;                 
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;
  notify = atoi ( temp) ;
  i++ ;        

  for (j=0;i<(int)strlen(string);i++,j++ ) 
   {
    if ( string[i] == '\t' )
      break ;                 
    temp[j] = string[i] ;
   }
  temp[j] = (char) NULL ;
  subscribe = atoi ( temp) ;
  i++ ;        

  if ( strcmp (EDITION, "Limited")) 
   {  /* Standard och extended edition */
    if (( category == article->category ) && ( group == article->subject ) && subscribe )
     {             
       if ( strlen(article->from) > 0 )
         sprintf ( command, "%s -r%s -s\"%s\" %s < %s/message-%d-%d-%d.dat", 
                  MAIL, article->from, article->title, to, DB_PATH, 
                  article->category, article->subject, article->id ) ; 
       else
          sprintf ( command, "%s -s\"%s\" %s < %s/message-%d-%d-%d.dat", 
                  MAIL, article->title, to, DB_PATH, 
                  article->category, article->subject, article->id ) ; 
       system ( command ) ;    /*  anropa mail-klienten */
       }
    }
  else
  {   /* Limited edition */

    if (( group == article->subject ) && subscribe )
     {
       if ( strlen(article->from) > 0 )
         sprintf ( command, "%s -r%s -s\"%s\" %s < %s/message-%d-%d.dat", 
                  MAIL, article->from, article->title, to, DB_PATH, 
                  article->subject, article->id ) ; 
      else
         sprintf ( command, "%s  -s\"%s\" %s < %s/message-%d-%d.dat", 
                  MAIL, article->title, to, DB_PATH, 
                  article->subject, article->id ) ; 

       system ( command ) ;    /*  anropa mail-klienten */
       }
  }
   
 } /* end of while */

}   /* end of GroupMail */

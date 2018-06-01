/*******************************************************************************************

   Program: intra.c
   Version:  2.1.2.5
   Date:      01/03/10 

   written by Casimir Artmann. 1996-1998
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../cgi.h"
#include "../utils.h"
#include "intra.h"

/* Hämtar initieringsdata för CGI-script 
   globala variabler deklarerade i intra.h */

int InitIntra ( char *filename ) 

{

FILE *fp ;
char string[255] ;
char temp[255];

  /* textfil för meddelanden */

int i,j ;

 /* om filename != NULL så hämta path för initieringsfil, avsedd för NT
    /*    filen finns i samma katalog som programmet */

 if ( filename[1] == ':' )   /* Running on Windows NT */
  {
    i = strlen(filename) ;
    for (;i>0;i-- )
      {
       if (( filename[i] == '\\' ) ||( filename[i] == '/' ))
        { 
         filename[i] = (char) NULL ;
         break ;}
        }
  filename[0] = (char) NULL ;

  strcat ( filename,"\\intra.ini") ;
  strcat ( MESSAGES, "\\messages.txt") ;
  }
 else   /* UNIX */
 {
  strcpy ( MESSAGES, "messages.txt") ; 
  strcpy ( filename, "intra.ini") ;
 }

if ((fp = fopen (filename,"r")) == NULL )
 {
  printf("Konfigurationsfilen %s kan ej öppnas\n", filename ) ;
  exit ( -1 ) ;
   }

while ( fgets ( string, 255, fp ) != NULL )
 {                    
 
  if ( !strncmp("ServerRoot ", string,11 ))
   {
     rem_sstring ( 0,11, string ) ;
     for (i=0,j=0;i<(int)strlen(string);i++,j++)
       if ( string[i] > ' ' )
         SERVER_ROOT[j] = string[i] ;
     SERVER_ROOT[j] = (char) NULL ;
   }                   
   
   
   if ( !strncmp("Router ", string,7 ))
   {
     rem_sstring ( 0,7, string ) ;
     for (i=0,j=0;i<(int)strlen(string);i++,j++)
       if ( string[i] > ' ' )
         ROUTER[j] = string[i] ;
     ROUTER[j] = (char) NULL ;
   }                   

if ( !strncmp("Access ", string,7 ))
   {
     rem_sstring ( 0,7, string ) ;
     for (i=0,j=0;i<(int)strlen(string);i++,j++)
       if ( string[i] > ' ' )
         ACCESS[j] = string[i] ;
     ACCESS[j] = (char) NULL ;
   }                   


  if ( !strncmp("ScriptAlias ", string,12 ))
   {
     rem_sstring ( 0,12, string ) ;
     for (i=0,j=0;i<(int)strlen(string);i++,j++)
       if ( string[i] > ' ' )
         CGI_PATH[j] = string[i] ;
     CGI_PATH[j] = (char) NULL ;
   } 
  if ( !strncmp("Database ", string,9 ))
   {
     rem_sstring ( 0,9, string ) ;
     for (i=0,j=0;i<(int)strlen(string);i++,j++)
       if ( string[i] > ' ' )
         DB_PATH[j] = string[i] ;
     DB_PATH[j] = (char) NULL ;
   } 

  if ( !strncmp("Version ", string,8 ))
   {
     rem_sstring ( 0,8, string ) ;
     for (i=0,j=0;i<(int)strlen(string);i++,j++)
       if ( string[i] > ' ' )
         VERSION[j] = string[i] ;
     VERSION[j] = (char) NULL ;
   } 

  if ( !strncmp("Edition ", string,8 ))
   {
     rem_sstring ( 0,8, string ) ;
     for (i=0,j=0;i<(int)strlen(string);i++,j++)
       if ( string[i] > ' ' )
         EDITION[j] = string[i] ;
     EDITION[j] = (char) NULL ;
   } 

  if ( !strncmp("Directory ", string,10 ))
   {
     rem_sstring ( 0,10, string ) ;
     for (i=0,j=0;i<(int)strlen(string);i++,j++)
       if ( string[i] > ' ' )
         HTML_PATH[j] = string[i] ;
     HTML_PATH[j] = (char) NULL ;
   } 

  if ( !strncmp("Icons ", string,6 ))
   {
     rem_sstring ( 0,6, string ) ;
     for (i=0,j=0;i<(int)strlen(string);i++,j++)
       if ( string[i] > ' ' )
         ICON_PATH[j] = string[i] ;
     ICON_PATH[j] = (char) NULL ;
   } 

  if ( !strncmp("Header ", string,7 ))
   {
     rem_sstring ( 0,7, string ) ;
     for (i=0,j=0;i<(int)strlen(string);i++,j++)
       if ( string[i] > ' ' )
         HEADER[j] = string[i] ;
     HEADER[j] = (char) NULL ;
   } 
  if ( !strncmp("Footer ", string,7 ))
   {
     rem_sstring ( 0,7, string ) ;
     for (i=0,j=0;i<(int)strlen(string);i++,j++)
       if ( string[i] > ' ' )
         FOOTER[j] = string[i] ;
     FOOTER[j] = (char) NULL ;
   } 

  if ( !strncmp("Frames ", string,7 ))
   {
     rem_sstring ( 0,7, string ) ;
     if ( !strncmp ("TRUE", string, 4 )) 
       FRAMES = 1 ;
       
   } 

  if ( !strncmp("Mail ", string,5 ))
   {
     rem_sstring ( 0,5, string ) ;
     for (i=0,j=0;i<(int)strlen(string);i++,j++)
       if ( string[i] > ' ' )
         MAIL[j] = string[i] ;
     MAIL[j] = (char) NULL ;
   } 

  if ( !strncmp("Executable ", string,11 ))
   {
     rem_sstring ( 0,11, string ) ;
     for (i=0,j=0;i<(int)strlen(string);i++,j++)
       if ( string[i] > ' ' )
         EXECUTABLE[j] = string[i] ;
     EXECUTABLE[j] = (char) NULL ;
   } 
 
  if ( !strncmp("Language ", string, 9 ))
   {
     rem_sstring ( 0,8, string ) ;
     for (i=0,j=0;i<(int)strlen(string);i++,j++)
       if ( string[i] > ' ' )
         LANGUAGE[j] = string[i] ;
     LANGUAGE[j] = (char) NULL ;
     strcpy ( LANGUAGE , "Svenska") ;
   } 

  if ( !strncmp("Locale ", string, 6 ))
   {
     rem_sstring ( 0,8, string ) ;
     for (i=0,j=0;i<(int)strlen(string);i++,j++)
       if ( string[i] > ' ' )
         temp[j] = string[i] ;
     temp[j] = (char) NULL ;
	 LOCALE = atoi ( temp ) ;
               LOCALE = 46 ;
   } 

 }

fclose ( fp ) ;

return ( 0 ) ;

} /* end of InitIntra */

int writeErrorLog ( char *string ) 

{

char timestamp [26] ;
char filename[128] ; 

time_t today ;
FILE *logfile ;

sprintf( filename, "%s/error.log" , DB_PATH ) ;

if (( logfile = fopen ( filename, "a" )) == NULL )
  exit ( -1 ) ;
	
time( &today ) ;
strftime( timestamp, 26,"%Y-%m-%d %H:%M:%S", localtime(&today) ) ;
fprintf (logfile, "%s\t%s\n", timestamp, string) ;
fclose ( logfile ) ;

}


int writeAccessLog ( struct _CGI CGI, char *module, int method, int category_id, int subject_id, int object_id ) 

{

char timestamp [26] ;
char filename[128] ; 

time_t today ;
FILE *logfile ;

sprintf( filename, "%s/access.log" , DB_PATH ) ;

if (( logfile = fopen ( filename, "a" )) == NULL )
  exit ( -1 ) ;
	
time( &today ) ;
strftime( timestamp, 26,"%Y-%m-%d %H:%M:%S", localtime(&today) ) ;
fprintf (logfile, "%s\t%s\t%s\t%s\t%sv%d\t%d\t%d\t%d\n", timestamp, CGI.remote_user, CGI.remote_addr, CGI.remote_host, module, method, category_id, subject_id, object_id ) ;
fclose ( logfile ) ;

}

int validNetwork ( struct _CGI CGI )

 /* check if valid network adress */
 
{

 /* check for invalid ip-adresses */


 /* if access = local */


 /* if access = internal */
 

 /* if access = external */
 
 
 return ( ERROR ) ;
 
}


int validUser (int iUserId, int iInformationClass, int iMethod, int iOwner, int iGroup ) 

{

 /* Om informationclass undefined then return error */
   
if ( iInformationClass == UNDEFINED )
  return ( ERROR ) ;

  /* Om informationsklass är EXTERNAL så är det fritt att läsa och skriva som annonym användare 
	 att ändra eller ta bort är ej tillåtet 
	 
				ANNONYMOUS		REGISTRED		MEMBER		OWNER		LIBRARIAN	ADMINISTRATOR
	SELECT			yes				yes			yes			yes							yes
	INSERT			yes				yes			yes			yes							yes
	UPDATE			no				no			no			yes							yes
	DELETE			no				no			no			yes							yes	
	EDIT			yes				yes			yes			yes							yes
	COMMENT			yes				yes			yes			yes							yes	
	COPY			no				no			no			yes							yes
	MOVE			no				no			no			yes							yes
*/
  
if ( iInformationClass == EXTERNAL )
  {
    if ( iUserId == -1 )
	  {
		if ( iMethod == SELECT ) return ( TRUE ) ;
		if ( iMethod == INSERT ) return ( TRUE ) ;
		if ( iMethod == UPDATE ) return ( FALSE ) ;
		if ( iMethod == DELETE ) return ( FALSE ) ;
		if ( iMethod == EDIT ) return ( TRUE ) ;
		if ( iMethod == COMMENT ) return ( TRUE ) ;
		if ( iMethod == COPY ) return ( FALSE ) ;
		if ( iMethod == MOVE ) return ( FALSE ) ;
	  }

	if ( iUserId == 0 ) 
	  {
		if ( iMethod == SELECT ) return ( TRUE ) ;
		if ( iMethod == INSERT ) return ( TRUE ) ;
		if ( iMethod == UPDATE ) return ( TRUE ) ;
		if ( iMethod == DELETE ) return ( TRUE ) ;
		if ( iMethod == EDIT ) return ( TRUE ) ;
		if ( iMethod == COMMENT ) return ( TRUE ) ;
		if ( iMethod == COPY ) return ( TRUE ) ;
		if ( iMethod == MOVE ) return ( TRUE ) ;
	  }

   if ( iUserId > 0 )
	  { 
	    if ( iUserId == iOwner )    /* ägare till objektet */
		  {
		   if ( iMethod == SELECT ) return ( TRUE ) ;
		   if ( iMethod == INSERT ) return ( TRUE ) ;
		   if ( iMethod == UPDATE ) return ( TRUE ) ;
		   if ( iMethod == DELETE ) return ( TRUE ) ;
		   if ( iMethod == EDIT ) return ( TRUE ) ;
		   if ( iMethod == COMMENT ) return ( TRUE ) ;
		   if ( iMethod == COPY ) return ( TRUE ) ;
		   if ( iMethod == MOVE ) return ( TRUE ) ;
		  }
		else						/* icke Šgare till objectet */
		  { 
			if ( iMethod == SELECT ) return ( TRUE ) ;
			if ( iMethod == INSERT ) return ( TRUE ) ;
			if ( iMethod == UPDATE ) return ( FALSE ) ;
			if ( iMethod == DELETE ) return ( FALSE ) ;
			if ( iMethod == EDIT ) return ( TRUE ) ;
			if ( iMethod == COMMENT ) return ( TRUE ) ;
			if ( iMethod == COPY ) return ( FALSE ) ;
			if ( iMethod == MOVE ) return ( FALSE ) ;	  
			}
		  }  /* iUserId > 0 */

   return ( ERROR ) ;
  }
  
    /* Om informationsklass Šr PUBLIC sŒ Šr det fritt att lŠsa som annonym anvŠndare 
	 att lŠgga till eller Šndra eller ta bort Šr ej tillŒtet 
	 
				ANNONYMOUS		REGISTRED		MEMBER		OWNER		LIBRARIAN	ADMINISTRATOR
	SELECT			yes				yes			yes			yes							yes
	INSERT			no				yes			yes			yes							yes
	UPDATE			no				no			no			yes							yes
	DELETE			no				no			no			yes							yes	
	EDIT			no				yes			yes			yes							yes
	COMMENT			yes				yes			yes			yes							yes	
	COPY			no				no			no			yes							yes
	MOVE			no				no			no			yes							yes
*/


if ( iInformationClass == PUBLIC )
{
     if ( iUserId == -1 )
	  {
		if ( iMethod == SELECT ) return ( TRUE ) ;
		if ( iMethod == INSERT ) return ( FALSE ) ;
		if ( iMethod == UPDATE ) return ( FALSE ) ;
		if ( iMethod == DELETE ) return ( FALSE ) ;
		if ( iMethod == EDIT ) return ( FALSE ) ;
		if ( iMethod == COMMENT ) return ( TRUE ) ;
		if ( iMethod == COPY ) return ( FALSE ) ;
		if ( iMethod == MOVE ) return ( FALSE ) ;
	  }

	if ( iUserId == 0 ) 
	  {
		if ( iMethod == SELECT ) return ( TRUE ) ;
		if ( iMethod == INSERT ) return ( TRUE ) ;
		if ( iMethod == UPDATE ) return ( TRUE ) ;
		if ( iMethod == DELETE ) return ( TRUE ) ;
		if ( iMethod == EDIT ) return ( TRUE ) ;
		if ( iMethod == COMMENT ) return ( TRUE ) ;
		if ( iMethod == COPY ) return ( TRUE ) ;
		if ( iMethod == MOVE ) return ( TRUE ) ;
	  }

   if ( iUserId > 0 )
	  { 
	    if ( iUserId == iOwner )    /* Šgare till objektet */
		  {
		   if ( iMethod == SELECT ) return ( TRUE ) ;
		   if ( iMethod == INSERT ) return ( TRUE ) ;
		   if ( iMethod == UPDATE ) return ( TRUE ) ;
		   if ( iMethod == DELETE ) return ( TRUE ) ;
		   if ( iMethod == EDIT ) return ( TRUE ) ;
		   if ( iMethod == COMMENT ) return ( TRUE ) ;
		   if ( iMethod == COPY ) return ( TRUE ) ;
		   if ( iMethod == MOVE ) return ( TRUE ) ;
		  }
		else						/* icke Šgare till objectet */
		  { 
			if ( iMethod == SELECT ) return ( TRUE ) ;
			if ( iMethod == INSERT ) return ( TRUE ) ;
			if ( iMethod == UPDATE ) return ( FALSE ) ;
			if ( iMethod == DELETE ) return ( FALSE ) ;
			if ( iMethod == EDIT ) return ( TRUE ) ;
			if ( iMethod == COMMENT ) return ( TRUE ) ;
			if ( iMethod == COPY ) return ( FALSE ) ;
			if ( iMethod == MOVE ) return ( FALSE ) ;	  
			}
		  }  /* iUserId > 0 */

   return ( ERROR ) ; 
  }
  
    /* Om informationsklass Šr INTERNAL sŒ Šr det ej tillŒtet som annonym anvŠndare
	 
				ANNONYMOUS		REGISTRED		MEMBER		OWNER		LIBRARIAN	ADMINISTRATOR
	SELECT			no				yes			yes			yes				yes			yes
	INSERT			no				yes			yes			yes				yes			yes
	UPDATE			no				no			no			yes				yes			yes
	DELETE			no				no			no			yes				yes			yes	
	EDIT			no				yes			yes			yes				yes			yes
	COMMENT			no				yes			yes			yes				yes			yes	
	COPY			no				no			no			yes				yes			yes
	MOVE			no				no			no			yes				yes			yes
*/

if ( iInformationClass == INTERNAL )
{
    if ( iUserId == -1 )
      return ( FALSE ) ;

	if ( iUserId == 0 ) 
	  {
		if ( iMethod == SELECT ) return ( TRUE ) ;
		if ( iMethod == INSERT ) return ( TRUE ) ;
		if ( iMethod == UPDATE ) return ( TRUE ) ;
		if ( iMethod == DELETE ) return ( TRUE ) ;
		if ( iMethod == EDIT ) return ( TRUE ) ;
		if ( iMethod == COMMENT ) return ( TRUE ) ;
		if ( iMethod == COPY ) return ( TRUE ) ;
		if ( iMethod == MOVE ) return ( TRUE ) ;
	  }

   if ( iUserId > 0 )
	  { 
	    if ( iUserId == iOwner )    /* Šgare till objektet */
		  {
		   if ( iMethod == SELECT ) return ( TRUE ) ;
		   if ( iMethod == INSERT ) return ( TRUE ) ;
		   if ( iMethod == UPDATE ) return ( TRUE ) ;
		   if ( iMethod == DELETE ) return ( TRUE ) ;
		   if ( iMethod == EDIT ) return ( TRUE ) ;
		   if ( iMethod == COMMENT ) return ( TRUE ) ;
		   if ( iMethod == COPY ) return ( TRUE ) ;
		   if ( iMethod == MOVE ) return ( TRUE ) ;
		  }
		else						/* icke Šgare till objectet */
		  { 
			if ( iMethod == SELECT ) return ( TRUE ) ;
			if ( iMethod == INSERT ) return ( TRUE ) ;
			if ( iMethod == UPDATE ) return ( FALSE ) ;
			if ( iMethod == DELETE ) return ( FALSE ) ;
			if ( iMethod == EDIT ) return ( TRUE ) ;
			if ( iMethod == COMMENT ) return ( TRUE ) ;
			if ( iMethod == COPY ) return ( FALSE ) ;
			if ( iMethod == MOVE ) return ( FALSE ) ;	  
			}

		  }  /* iUserId > 0 */
  return ( ERROR ) ;
}

    /* Om informationsklass Šr CONFIDENTIAL sŒ Šr det fritt att lŠsa som annonym anvŠndare 
	 att lŠgga till eller Šndra eller ta bort Šr ej tillŒtet 
	 
				ANNONYMOUS		REGISTRED		MEMBER		OWNER		LIBRARIAN	ADMINISTRATOR
	SELECT			no				no			yes			yes				yes			yes
	INSERT			no				no			yes			yes				yes			yes
	UPDATE			no				no			no			yes				yes			yes
	DELETE			no				no			no			yes				yes			yes	
	EDIT			no				no			yes			yes				yes			yes
	COMMENT			no				yes			yes			yes				yes			yes	
	COPY			no				no			no			yes				yes			yes
	MOVE			no				no			no			yes				yes			yes
*/

if ( iInformationClass == CONFIDENTIAL )
{
 if ( iUserId == -1 )
      return ( FALSE ) ;

	if ( iUserId == 0 ) 
	  {
		if ( iMethod == SELECT ) return ( TRUE ) ;
		if ( iMethod == INSERT ) return ( TRUE ) ;
		if ( iMethod == UPDATE ) return ( TRUE ) ;
		if ( iMethod == DELETE ) return ( TRUE ) ;
		if ( iMethod == EDIT ) return ( TRUE ) ;
		if ( iMethod == COMMENT ) return ( TRUE ) ;
		if ( iMethod == COPY ) return ( TRUE ) ;
		if ( iMethod == MOVE ) return ( TRUE ) ;
	  }
	  
	if ( iUserId == 1 ) 
	  {
		if ( iMethod == SELECT ) return ( TRUE ) ;
		if ( iMethod == INSERT ) return ( TRUE ) ;
		if ( iMethod == UPDATE ) return ( TRUE ) ;
		if ( iMethod == DELETE ) return ( TRUE ) ;
		if ( iMethod == EDIT ) return ( TRUE ) ;
		if ( iMethod == COMMENT ) return ( TRUE ) ;
		if ( iMethod == COPY ) return ( TRUE ) ;
		if ( iMethod == MOVE ) return ( TRUE ) ;
	  }

   if ( iUserId > 0 )
	  { 
	    if ( iUserId == iOwner )    /* Šgare till objektet */
		  {
		   if ( iMethod == SELECT ) return ( TRUE ) ;
		   if ( iMethod == INSERT ) return ( TRUE ) ;
		   if ( iMethod == UPDATE ) return ( TRUE ) ;
		   if ( iMethod == DELETE ) return ( TRUE ) ;
		   if ( iMethod == EDIT ) return ( TRUE ) ;
		   if ( iMethod == COMMENT ) return ( TRUE ) ;
		   if ( iMethod == COPY ) return ( TRUE ) ;
		   if ( iMethod == MOVE ) return ( TRUE ) ;
		  }
		else						/* icke Šgare till objectet */
		  { 
		    if (( iUserId == 2) || ( iUserId == 5) || ( iUserId == 11)) return ( TRUE ) ;
			 
			if ( iMethod == SELECT ) return ( FALSE ) ;
			if ( iMethod == INSERT ) return ( FALSE ) ;
			if ( iMethod == UPDATE ) return ( FALSE ) ;
			if ( iMethod == DELETE ) return ( FALSE ) ;
			if ( iMethod == EDIT ) return ( FALSE ) ;
			if ( iMethod == COMMENT ) return ( TRUE ) ;
			if ( iMethod == COPY ) return ( FALSE ) ;
			if ( iMethod == MOVE ) return ( FALSE ) ;	  
			}

		  }  /* iUserId > 0 */

return ( ERROR ) ;
  }
  


  /* Om informationsklass Šr SECRET sŒ Šr det fritt att lŠsa som annonym anvŠndare 
	 att lŠgga till eller Šndra eller ta bort Šr ej tillŒtet 
	 
				ANNONYMOUS		REGISTRED		MEMBER		OWNER		LIBRARIAN	ADMINISTRATOR
	SELECT			no				no			yes			yes				no			yes
	INSERT			no				no			yes			yes				no			yes
	UPDATE			no				no			no			yes				no			yes
	DELETE			no				no			no			yes				no			yes	
	EDIT			no				no			yes			yes				no			yes
	COMMENT			no				no			yes			yes				no			yes	
	COPY			no				no			no			yes				no			yes
	MOVE			no				no			no			yes				no			yes
	
*/

if ( iInformationClass == SECRET )
{

 if ( iUserId == -1 )
      return ( FALSE ) ;

	if ( iUserId == 0 ) 
	  {
		if ( iMethod == SELECT ) return ( TRUE ) ;
		if ( iMethod == INSERT ) return ( TRUE ) ;
		if ( iMethod == UPDATE ) return ( TRUE ) ;
		if ( iMethod == DELETE ) return ( TRUE ) ;
		if ( iMethod == EDIT ) return ( TRUE ) ;
		if ( iMethod == COMMENT ) return ( TRUE ) ;
		if ( iMethod == COPY ) return ( TRUE ) ;
		if ( iMethod == MOVE ) return ( TRUE ) ;
	  }

   if ( iUserId > 0 )
	  { 
	    if ( iUserId == iOwner )    /* Šgare till objektet */
		  {
		   if ( iMethod == SELECT ) return ( TRUE ) ;
		   if ( iMethod == INSERT ) return ( TRUE ) ;
		   if ( iMethod == UPDATE ) return ( TRUE ) ;
		   if ( iMethod == DELETE ) return ( TRUE ) ;
		   if ( iMethod == EDIT ) return ( TRUE ) ;
		   if ( iMethod == COMMENT ) return ( TRUE ) ;
		   if ( iMethod == COPY ) return ( TRUE ) ;
		   if ( iMethod == MOVE ) return ( TRUE ) ;
		  }
		else						/* icke Šgare till objectet */
		  { return ( FALSE ) ;	  
			}

		  }  /* iUserId > 0 */
		  
return ( ERROR ) ;

}   
return ( ERROR ) ;

}


   /*
   Expand method plockar fram raden method=xxxxxx  
   från ett inläst formulär. Följande värden är godkända 
      
      select
      insert
      update
      delete
      edit
      COMMENT
      copy
      move


      om method saknas returnerar funktionen FALSE 
        och method sätts till -1
            
      */

int ExpandMethod ( struct _CGIarg *CGIarg, int *method ) 

{

struct _CGIarg *first ;

  /* spara en pekare till första argumentet i COMMENTan */ 
  
first = malloc ( sizeof( *CGIarg )) ; 
first = CGIarg ; 

*method = -1 ;

while ( CGIarg != NULL )
 {
  if ( !strncmp("method", CGIarg->name, 6 ))
   {
    if ( !strncmp ( "select", CGIarg->value,6 ))
      *method = SELECT ;
    if ( !strncmp ( "insert", CGIarg->value,6 ))
      *method = INSERT ;
    if ( !strncmp ( "update", CGIarg->value,6 ))
      *method = UPDATE ;
    if ( !strncmp ( "delete", CGIarg->value,6 ))
      *method = DELETE ;  
    if ( !strncmp ( "edit", CGIarg->value,4 ))
      *method = EDIT ;
    if ( !strncmp ( "COMMENT", CGIarg->value,4 ))
      *method = COMMENT ;
   if ( !strncmp ( "copy", CGIarg->value,4 ))
      *method = COPY ;
   if ( !strncmp ( "move", CGIarg->value,4 ))
      *method = MOVE ;
    if ( *method >= 0 )    /* giltig metod */
      break ;
        
   }
    
  CGIarg = CGIarg->next ;
 }

CGIarg = first ;

/*free ( first ) ;*/

if ( *method != -1 ) 
  return ( TRUE ) ;
else
  return ( FALSE ) ;
    
}   /* end of method */





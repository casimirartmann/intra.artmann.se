
/*
    cgi.c - 2.1   1998-09-04
    
    Funktioner f�r hantering av CGI-script
    
    GetCGI	- H�mtar v�rden med method=get	 
    HTMLf   - skriv till stdout formaterat med HTML-kod
    sHTMLf  - skriv till str�ng formaterar med HTML-kod
    catHTML - skriv ut angiven textfil som HTML-kod
    errorHTML - skriv ut ett felmeddelande med HTML-kod
    
    bURLencode - konverterar en st�ng till en lista med variabler och v�rden 
   
    version 1.0 - 
    version 1.1 - Namn bytt till fr�n cgi-bin till cgi.c
    version 1.2 - Ytterligare funktioner f�r HTML-formatering
    version 2.0 - Anrop till bURLencode �ndrat
                  Koden uppdelad i cgi.h och cgi.c
                  Ej WWW-baserade funktioner flyttade till utils.c
    version 2.1 - SCCS st�d, div buggr�ttningar
                 2.1.1.2 - Minnesl�cka i bURLencode
	version 2.1.3 - r�ttningar f�r OS/X
	



*/

/*    */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#include "utils.h"
#include "cgi.h"

void InitCGI () 

{

printf("Content-type: text/html\n\n");
 /* kontrollera att anrop g�rs via CGI-script */

/*
if ( getenv("REQUEST_METHOD") == NULL )
 { 
  printf ("REQUEST_METHOD saknas\n") ;
  exit ( -1 ) ;
 }
*/

}

void GetCGI ( char **instring, struct _CGI *CGI ) 

{

char *string ;
int size ;
int x ;

char timestamp [26] ;
char filename[128] ; 

time_t today ;
FILE *logfile ;
 
  /* hur mycket minne skall allokeras till inbuffer */

char *request_method = getenv("REQUEST_METHOD") ;
char *content_lenght = getenv("CONTENT_LENGTH") ;
char *query_string = getenv("QUERY_STRING") ;

char *auth_type = getenv ("AUTH_TYPE") ;
char *http_user_agent = getenv ("HTTP_USER_AGENT") ;

char *script_name = getenv ("SCRIPT_NAME") ;
char *path_info = getenv("PATH_INFO") ;

char *remote_addr = getenv("REMOTE_ADDR") ;
char *remote_user = getenv("REMOTE_USER") ;
char *remote_host = getenv("REMOTE_HOST") ;
char *remote_ident = getenv("REMOTE_IDENT") ;

char *server_software = getenv ("SERVER_SOFTWARE") ;
char *server_name = getenv ("SERVER_NAME") ;
char *server_port = getenv ("SERVER_PORT") ;
char *server_protocol = getenv ("SERVER_PROTOCOL") ;

if ( content_lenght != NULL)
  size = atoi( content_lenght) ;  
else
  size = 256 ;

  /* reservera minne, om minne inte rdcket till se avsluta programmet */

*instring = malloc (size) ; 
if ( *instring == NULL )
  { printf("Not enough memory\n") ;
    exit (-1) ; }
string = malloc (size) ;

if ( string == NULL )
  { printf("Not enough memory\n") ;
    exit (-1) ; }

  /* anrop �r godk�nt  */
  /* l�s in variabler beroende p� typ av anrop samt �vriga v�rden */

CGI->auth_type[0] = (char) NULL ;
CGI->user_agent[0] = (char) NULL ;
CGI->script_name[0] = (char) NULL ;
CGI->path_info[0] = (char) NULL ;
CGI->remote_user[0] = (char) NULL ;
CGI->remote_host[0] = (char) NULL ;
CGI->remote_addr[0] = (char) NULL ;
CGI->remote_ident[0] = (char) NULL ;
CGI->server_name[0] = (char) NULL ;
CGI->server_protocol[0] = (char) NULL ;
CGI->server_port[0] = (char) NULL ;
CGI->server_software[0] = (char) NULL ;

if ( auth_type != NULL ) 
  strcpy ( CGI->auth_type, auth_type ) ; 
if ( http_user_agent != NULL )
 strcpy ( CGI->user_agent, http_user_agent ) ; 
if ( script_name != NULL )
 strcpy ( CGI->script_name, script_name ) ;  
if ( path_info != NULL )
 strcpy ( CGI->path_info, path_info ) ;  

if ( remote_user != NULL )
 strcpy ( CGI->remote_user, remote_user ) ;
if ( remote_host != NULL )
 strcpy ( CGI->remote_host, remote_host ) ;
if ( remote_addr != NULL )
 strcpy ( CGI->remote_addr, remote_addr ) ;
if ( remote_ident != NULL )
 strcpy ( CGI->remote_ident, remote_ident ) ;

if ( server_name != NULL )
 strcpy ( CGI->server_name, server_name ) ;
if ( server_protocol != NULL )
 strcpy ( CGI->server_protocol, server_protocol ) ;
if ( server_port != NULL )
 strcpy ( CGI->server_port, server_port ) ;   
if ( server_software != NULL )
 strcpy ( CGI->server_software, server_software ) ;

	
/*

*/

if ( request_method == NULL )
  { printf ("Unknown request method<br>\n") ;
    exit ( 0 ) ;
	}    
  if ( !strcmp ( request_method,"GET") )   /* method = GET */
   {  
  
	  CGI->request = 1 ;
      /* finns variabler att l�sa in? */
      if ( query_string == NULL )
        {  
          printf("Missing input values\n") ;
          exit ( 0 ) ;
         }

    *instring = getenv ("QUERY_STRING") ;
   
    }  /* end of if request_method = 1 */

   if ( !strcmp ( request_method,"POST")  )   /* method = POST */
    {
       CGI->request = 2 ;
	   
	     /* hur m�nga tecken skall �sas in?
          �ndra storlek p� instring */
      
        /* l�s in alla tecken */

      for ( x=0; x < size; x++ )
         string[x] = fgetc(stdin) ;
       
      string[x] = (char) NULL ;
      if ( string == NULL ) {
        printf("No query information to decode\n" ) ;
        exit ( FALSE ) ;
       } 
      strcpy ( *instring, string ) ;         
       /* free (string) ;  */

    }       /* end of request_method = 2 */

return ; 

}

 /* funktionen konverterar inl�st str�ng till argument f�r CGI-script */

int GetCGIargs ( char *string, struct _CGIarg *CGIarg )


{

int counter ;

counter = 0 ;


return ( counter ) ;

}   /* end of GetCGIargs */

void sHTMLf ( char *outstring, char *instring)  

{

int i,j ;

for ( i=0,j=0; i < (int)strlen(instring); i++,j++ ) 
 {
    outstring[j] = instring[i] ;
 }
}


void printfHTML ( char *instring ) 

{

int i ;

for ( i=0; i < (int)strlen(instring); i++ ) 
 {
  printf("%c", instring[i] ) ;
  if ( instring[i] == '\n' )
    printf("<br>") ;
 }
}


void fprintfHTML ( FILE *fp, char *instring )

{
int i ;

for ( i=0; i < (int)strlen(instring); i++ ) 
 {
    fprintf( fp, "%c", instring[i] ) ;
   if ( instring[i] == '\n' )
    fprintf(fp,"<br>") ;
 }
}

  /* skriver ut angiven textfil p� bildsk�men 
     om filen saknas skrivs ett felmeddelande ut som HTML-kod 
     max 255 tecken / rad 
  
  */

void errorHTML ( char *string ) 

{

 char timestamp [26] ;
 char filename[128] ; 

 time_t today ;
 FILE *logfile ;

 printf ( "<p><cite>%s</cite><p>\n", string ) ;
 
 if (( logfile = fopen ( "/usr/local/apache/db/error.log", "a" )) == NULL )
   exit ( -1 ) ;
	
 time( &today ) ;
 strftime( timestamp, 26,"%Y-%m-%d %H:%M:%S", localtime(&today) ) ;
 fprintf (logfile, "%s %s\n", timestamp, string ) ;
 fclose (logfile) ;
 
 exit ( 1 ) ;

}

     
void catHTML ( char *filename) 

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
    printf ("%s", string ) ;
   }

fclose ( fp ) ;

}




int bURLencode ( char *instring, struct _CGIarg *CGIarg )

{

 /* Omvandla till inl�sta v�rden fr�n HTLM-kod en lista med variabelnamn och v�rden */
 
 /* i r�knar upp aktuellt tecken som omvandlas
    j r�knar upp det konverterade tecknet 
    
    k r�knar texten som skall f�ras �ver till namn resp v�rde */

int i,j,k,l ;
int high, low ;
int counter = 0 ;
char *string ; 

struct _CGIarg *first ; /* f�rsta inl�gg */
struct _CGIarg *ptr ;  /* n�sta inl�gg */  


first = malloc ( sizeof ( struct _CGIarg )) ;
first = CGIarg ;
CGIarg->next = NULL ;

     
string = malloc ( strlen(instring)) ;

for (i=0,j=0; i<(int)strlen(instring); i++)
 {
  if (   (instring[i] == '%' ) &&
          ( instring[i+1] == '0' ) &&   /* Ny rad i textarea */
           ( instring[i+2] == 'D' ) &&
           ( instring[i+3] == '%' ) &&
           ( instring[i+4] == '0' ) &&
           ( instring[i+5] == 'A' ))
       {
        string[j] = '\n' ;  j++ ;
        i = i + 5 ;
       continue ;
       }
    

  if ( instring[i] != '%' )  /* Ej specialtecken */
   {
    if ( instring[i] == '+' )  /* Omvandla plus till blanktecken */
     {
      string[j] = ' ' ;
      j ++ ;
     }
    else
     {   /* Omvandla & till ny post - radbrytning eller slut p� inmatning */
	  if ( instring[i] == '&' )
       {
        string[j] = (char) NULL ;
        
         /* dela upp str�ng i name och value */     
         
        for (k=0;string[k] != '=';k++)      /* name=xxxx */
          CGIarg->name[k] = string[k] ;
        CGIarg->name[k] = (char) NULL ;
        for (l=0; l<strlen ( CGIarg->name ); l++) 
          CGIarg->name[l] = tolower (CGIarg->name[l] ) ;


        k++ ;
        
        rem_sstring ( 0, k, string ) ;      /* alla tecken efter = till value */
                                            /* reservera plats f�r alla tecken */                                            
        
        CGIarg->value = malloc ( strlen ( string )+1) ;
        strncpy ( CGIarg->value, string, strlen(string) ) ;  
        CGIarg->value[strlen(string)] = (char) NULL ;
        
        string[j] = (char) NULL ; j = 0 ;   /* nollst�ll efter att f�r �ver v�rden */
      
       ptr = malloc ( sizeof ( struct _CGIarg )) ;  /* adress till n�sta post */
       ptr->next = NULL ;                             /* efterf�ljande adress saknas */
       CGIarg->next = ptr ;                        /* l�gg till adress till befintlig post */
       CGIarg = ptr ;                              /* ny adress f�r
 post */
 
     
        counter ++ ;                      /* antal poster */
           
        
       }      
	  else
	   {                         /* �vriga tecken */
	    string[j] = instring[i] ;
        j++ ;
       }
	 }
   }
  else
   {
	  /* OBS Omvandling till teckenupps�ttning beroende p� operativsystem */
         /* omvandlar hex-kod i teckenstr�ng till ett decimaltal */

      high =  instring[i+1] ;
	  if ( high > '9' )
	    high = high - 55 ;
	  else
		high = high - 48 ;

	  low = instring[i+2] ;
      if ( low > '9' )
	    low = low - 55 ;
	  else
	    low = low - 48 ;

      string[j] = ( high * 16 ) + low ;
	  j ++ ; 
      i = i + 2 ;
	 }   /* end of if .. else .. */
   
 }  /* end of for */


        /* hantering av sista v�rdepar */
        
        string[j] = (char) NULL ;
        
         /* dela upp str�ng i name och value */     
         
        for (k=0; k < (int)strlen(string);k++)      /* name=xxxx */
         {
          if ( string[k] == '=' )
            break ;
          CGIarg->name[k] = string[k] ;
         }

        CGIarg->name[k] = (char) NULL ;
        for (l=0; l<strlen ( CGIarg->name ); l++) 
          CGIarg->name[l] = tolower (CGIarg->name[l] ) ;

        k++ ;
        
        rem_sstring ( 0, k, string ) ;      /* alla tecken efter = till value */
                                            /* reservera plats f�r alla tecken */                                            

        CGIarg->value = malloc ( strlen ( string )+1) ;
        strncpy ( CGIarg->value, string, strlen(string) ) ;  
        CGIarg->value[strlen(string)] = (char) NULL ;


       
CGIarg = first ;   /*pekare till f�rsta posten */
/*free ( first ) ;    fungerar ej f�r linux    */   

return ( TRUE ) ;

}  /* end of bURLencode */ 




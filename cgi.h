struct _CGI {   /* innehåller miljövariabler för CGI-anrop */
int content ;
int request ;      /* GET = 1, POST = 2, HEAD = 3 */
char query_string[255] ;  /* query string */
char path_info[255] ;   /*              */
char auth_type[255] ;
char user_agent[255] ;  /* HTTP USER AGENT*/
char remote_addr[255] ;
char remote_host[255]  ;
char remote_user[255] ;
char remote_ident[255] ;
char script_name[255] ;
char server_name[255] ;
char server_port[255] ;
char server_protocol[255] ;
char server_software[255] ;

} ;


struct _CGIarg {  /* konverterade argument för CGI */

     char name[32] ;    /* namn får max innehålla 32 tecken */
     char *value ;      /* value kan vara hur lång som helst */
     struct _CGIarg *next ;   /* nästa argument */

} ;

void InitCGI () ;
void GetCGI  ( char **instring, struct _CGI *CGI ) ;
int GetCGIargs ( char *string, struct _CGIarg *CGIarg ) ;
int bURLencode ( char *instring, struct _CGIarg *CGIarg ) ;

void sHTMLf ( char *outstring, char *instring)  ;
void printfHTML ( char *instring ) ;
void fprintfHTML ( FILE *fp, char *instring ) ;
void errorHTML ( char *string ) ;
void catHTML ( char *filename) ;


        


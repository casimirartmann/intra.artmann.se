   /*************************************************** 
   
   intra.h


   *****************************************************/

/* S�tt globala startvariabler f�r programmet */

char SERVER_ROOT[255] ;
char ROUTER[255] ;
char ACCESS[255] ;
char DB_PATH[255] ;
char CGI_PATH[255] ;
char HTML_PATH[255] ;
char ICON_PATH[255] ;
char HEADER[255] ;
char FOOTER[255] ;
char MAIL[255] ;
char SMTP[255] ;
char EDITION[255] ; 
char VERSION[255] ;
char HTML[255] ;
char DB[255] ;
char EXECUTABLE[8] ;
char LANGUAGE[255] ;
int  LOCALE ;
char MESSAGES[255];
char HOMEPAGE[255] ;


int  FRAMES ;
int GRAPHICS ;


#define ERROR -1
#define DAYS 7 

/* Information security classes  */

#define UNDEFINED -1
#define EXTERNAL 0
#define PUBLIC 1
#define INTERNAL 2
#define CONFIDENTIAL 3
#define SECRET 4
#define PRIVATE 5
#define MAILBOX 6
#define BLOGG 7


 /* F�ljande konstanter anv�nds f�r att v�lja method  */
  
#define SELECT 0
#define INSERT 1
#define UPDATE 2
#define DELETE 3
#define EDIT 4
#define COMMENT 5
#define COPY 6
#define MOVE 7
#define VERIFY 8
#define APPROVE 9

/* sub-class for publications */

#define BOOK 1
#define MAGAZINE 2
#define NEWSPAPER 3
#define LP 4
#define AUDIO_TAPE 5
#define VIDEO_TAPE 6
#define DAT  7
#define CD 8
#define DVD 9

/* parametrar f�r en anv�ndare */

struct _user {    
  int id ;                  /* userid         */
  char  login[32] ;         /*  same as login */
  char  password[32] ; 
  char  name[64] ;          /*  full username */
  char  salutation[64] ;    /*  */  
  char  epost[128] ;        /*  e-mail adress */
  char  email[128] ;        /*                */
  char  home_email ;
  char  company[64] ;
  char  country[64] ;
  char  homepage[128] ;   
  char  phone[32] ;
  char  mobile[32] ;
  char  fax [32] ;
  char  zip[12] ;
  char  city[64] ;
  char  region[62] ;  
  char  adress[64] ;
} ;

struct _contact {    
  int id ;                  /* userid         */
  char  first_name[64] ;    /*  full username */
  char  second_name[64] ;   
  char  salutation[64] ;    /*  */  
  char  title[128] ;        /*                */
  char  company[64] ;
  char  company_email[128] ;
  char  company_phone[32] ;
  char  company_mobile[32] ;
  char  company_fax [32] ;
  char  company_zip[12] ;
  char  company_city[64] ;
  char  company_region[62] ;  
  char  company_adress[64] ;
  char  company_country[64] ;

  char  home_email[128] ;
  char  home_phone[32] ;
  char  home_mobile[32] ;
  char  home_fax [32] ;
  char  home_zip[12] ;
  char  home_city[64] ;
  char  home_region[62] ;  
  char  home_adress[64] ;
  char  home_country[64] ;
} ;

struct _category { 
        int id ;             /* nr p� nyhetsgrupp */
        char name[64] ;      /* namn p� nyhetsgrupp */
        char path[128];      /*                     */
		char server[128] ;   /**/
		char remark[255] ;   /* kommentar */
        int  security  ;      /* publik eller privat */
        int  owner ;         /* �gare till nyhetsgruppen */
        int  group ;             /* */
		char created[26] ; /* datum f�r senaste artikel */
		 struct _category *next ;   /* n�sta nyhetsgrupp */
		} ;
				
struct _subject {
        int category ;       /* katagori p� som nyhetsgruppen tillh�r */  
        int id ;             /* nr p� nyhetsgrupp */
        char name[64] ;      /* namn p� nyhetsgrupp */
        char remark[255] ;   /* kommentar */
        int  security  ;      /* publik eller privat */
        int  owner ;         /* �gare till nyhetsgruppen */
		int  group ;         /* grupp */
        char created[26] ; /* datum f�r senaste artikel */
        int article ;        /* senaste artikelnummer */ 
        struct _subject *next ;   /* n�sta nyhetsgrupp */
                } ; 


struct _title {
        int id ;
        int category ;			/* kategori p� som �nmet tillh�r */  
        int subject ;			/* nyhetsgrupp */
        int security ;			/* informationsklass */
        int object ;			/* kbjectklass f�r rubrik  */
        int subclass ;			/* sub-klass f�r rubrik */
        int owner ;				/* userid f�r �gare av meddelandet */
        int group ;				/* groupid f�r meddelandet */
        int indent ;			/* rubrikniv�  */   
		int language ;			/* spr�k */
		int status ;            /* life-cycle status */   
        char created[26] ;		/* skapad datum */
        char modified[26] ;		/* datum f�r modifiering */
        char name[64] ;			/* rubrik */
        char email[128] ;       /* e-post f�r redigerad av */
        char author[64] ;       /* f�rfattare */
        char company[64] ;      /*            */
		char href[256] ;		/*            */
        struct _title *next ;   /* n�sta rubrik */
        } ;                

struct _link {
        int category ;       /* kategori p� som nyhetsgruppen tillh�r */  
        int id ;
        int indent ;         /* rubrikniv�  */   
        int subject ;       /* �mne */
		int target ;           /* */
        int group ;
        int security ;          /* publikt eller skyddat */
        int owner ;            /* userid f�r �gare av meddelandet */
		int comment ;          /* f�reg�ende objekt */
        char href[255] ;       /* l�nk */  
        char title[64] ;       /* rubrik */   
        char email[128] ;       /* e-post f�r avs�ndare */
        char author[64] ;      /* avs�ndare */
        char created[26] ; 
        char company[64] ;   
        struct _link *next ; /* n�sta rubrik */
        } ;                
        
struct _message {
        int category ;       /* katagori p� som nyhetsgruppen tillh�r */  
        int id ;
        int indent ;         /* rubrikniv� */
        int subject ;      /* aktuellt �mne */
        int target ;             /* */
        int group ;
        int security ;
        int owner ;         
        char title[64] ;     /* rubrik */
        char author[64] ;    /* f�rfattare, klartext */
        char email[128] ;     /* e-postadress */
        char to[128] ;       /* mottagande e-postadress */
        char company[64] ;  /* f�retag f�r f�rfattare */
        int comment ;        /* tidigare meddelande i samma nyhetsgrupp */
        char reply[64] ;     /* rubrik p� tidigare meddelande */
        char created [26] ; /* ����-mm-dd tt:mm:ss */
        char text[4096] ;    /* text i meddelande */
        struct _message *next ;   /* n�sta artikel */        
} ;                                                      

struct _article {
        int category ;       /* katagori p� som nyhetsgruppen tillh�r */  
        int id ;
        int indent ;         /* rubrikniv� */
        int subject ;        /* aktuellt �mne  */
        int target ;         /* kopiera / flytta till */
        int group ;
        int security ;
        int owner ;         
        char title[64] ;     /* rubrik */
        char author[64] ;    /* f�rfattare, klartext */
        char email[128] ;     /* e-postadress */
        char to[128] ;       /* mottagande e-postadress */
        char company[64] ;   /* f�retag f�r f�rfattare */
        int  comment ;       /* tidigare meddelande i samma nyhetsgrupp */
        char reply[64] ;     /* rubrik p� tidigare meddelande */
        char created[26] ; /* ����-mm-dd tt:mm:ss */
        char text[4096] ;    /* text i meddelande */
        struct _article *next ;   /* n�sta artikel */        
} ;                                                      


struct _audio {
        int category ;       /* katagori p� som nyhetsgruppen tillh�r */  
        int subject ;        /* aktuellt �mne */
        int id ;
        int target ;             /* */
        int group ;
        int security ;
        int owner ;         
        char title[64] ;     /* rubrik */
        char author[64] ;    /* f�rfattare, klartext */
        char created[26] ; /* ����-mm-dd tt:mm:ss */
        char file[128] ;     /* filename */
	int indent ;
	int comment ;
	char email[64] ;
	char text[4096] ;
	char company[64] ;
	char reply[64] ;
        struct _audio *next ;   /* n�sta post */        
} ;     


struct _image {
        int category ;       /* katagori p� som nyhetsgruppen tillh�r */  
        int id ;
        int subject ;        /* aktuellt �mne */
        int target ;             /* */
        int group ;
        int security ;
        int owner ;         
        char title[64] ;     /* rubrik */
        char author[64] ;    /* f�rfattare, klartext */
        char created[26] ; /* ����-mm-dd tt:mm:ss */
        char file[128] ;     /* filename */
        struct _image *next ;   /* n�sta post */        
} ;     

struct _video {
        int category ;       /* katagori p� som nyhetsgruppen tillh�r */  
        int subject ;        /* aktuellt �mne */
        int id ;
        int target ;             /* */
        int group ;
        int security ;
        int owner ;         
        char title[64] ;     /* rubrik */
        char author[64] ;    /* f�rfattare, klartext */
        char created[26] ; /* ����-mm-dd tt:mm:ss */
        char file[128] ;     /* filename */
        struct _video *next ;   /* n�sta post */        
} ;     

struct _publication {
        int category ;       /* katagori p� som nyhetsgruppen tillh�r */  
        int id ;
        int indent ;         /* rubrikniv� */
        int subject ;        /* aktuellt �mne  */
        int target ;         /* kopiera / flytta till */
        int group ;
		int media ;
        int security ;
        int owner ;         
        char title[64] ;     /* rubrik */
        char author[64] ;    /* f�rfattare, klartext */
        char email[128] ;     /* e-postadress */
        char to[128] ;       /* mottagande e-postadress */
        char company[64] ;   /* f�retag f�r f�rfattare */
        int  comment ;       /* tidigare meddelande i samma nyhetsgrupp */
        char reply[64] ;     /* rubrik p� tidigare meddelande */
        char created[26] ; /* ����-mm-dd tt:mm:ss */
        char text[4096] ;    /* text i meddelande */
        struct _article *next ;   /* n�sta artikel */        
} ;                               
struct _document {
        int category ;       /* kategori p� som nyhetsgruppen tillh�r */  
        int id ;
        int indent ;         /* rubrikniv� */
        int conferance  ;    /* aktuellt konferanssystem */
        int subject ;      /* aktuell nyhetsgrupp */
        int target ;
        int group ;
        int security ;
        int owner ;         
        char title[64] ;     /* rubrik */
        char author[64] ;    /* f�rfattare, klartext */
        char verified[64] ;  /* kontrollerad */
        char approved[64] ;  /* godk�nd */
		char distribute[255] ; 
		char email[128] ;     /* e-postadress */
        char to[128] ;       /* mottagande e-postadress */
        char company[64] ;  /* f�retag f�r f�rfattare */
		int  subclass ;       
        char reply[64] ;     /* rubrik p� tidigare meddelande */
        char created[26] ; /* ����-mm-dd tt:mm:ss */
        char version[12] ;
		char href[128] ;
		char filename[64] ;  /* dokumentnummer */
		char text[4096] ;    /* text i meddelande */
        struct _document *next ;   /* n�sta artikel */        
} ;

  /* */
  /* 
  
  
  */
  
int InitConferance () ;
int InitIntra ( char *filename ) ;
int validUser ( int iUserId,  int iInformationClass, int iMethod, int iOwner, int iGroup ) ;

int printRequestLog ( struct _CGI CGI, char *module, int method, int category_id, int subject_id, int object_id ) ;


int ExpandMethod ( struct _CGIarg *CGIarg, int *method ) ;
int ExpandInbox ( struct _CGIarg *CGIarg, int *scope ) ;
int ExpandSubject ( struct _CGIarg *CGIarg, struct _subject *subject ) ;

int ExpandLink ( struct _CGIarg *CGIarg, struct _link *link ) ;
int ExpandArticle ( struct _CGIarg *CGIarg, struct _article *article ) ;
int ExpandMessage ( struct _CGIarg *CGIarg, struct _message *message ) ;

int ExpandImage ( struct _CGIarg *CGIarg, struct _image *image ) ;
int ExpandVideo ( struct _CGIarg *CGIarg, struct _video *video ) ;
int ExpandAudio ( struct _CGIarg *CGIarg, struct _audio *audio ) ;



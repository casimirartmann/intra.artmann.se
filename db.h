   /*  
      Module: %M%
      Version %I%
      Datum: %D%
	  
   */
   
int findUser ( char *name, struct _user *user ) ;

int checkCategory () ;
int getCategoryName ( int category, char *name ) ;
int getLastSubjectId ( int category ) ;
int getLastCategoryId ( ) ;

int selectCategory ( struct _category *category, char *show, char *sort, char *cursor ) ;
int insertCategory ( struct _category *category ) ;

int getSubjectName ( int category, int group, char *name ) ;
int getSubjectClass ( int category, int group ) ;
int getSubjectRemark ( int category, int group, char *name ) ;

/* */

FILE *lockSubject ( int x, int y ) ;
int unlockSubject ( FILE *lock  ) ;

int GetNextTitleId ( int x, int y ) ;
int GetNextArticleId ( int category, int id ) ;
int GetNextCategoryId ( int category ) ;

int insertTitle ( struct _title title ) ;
int updateTitle ( struct _title title ) ;
int deleteTitle ( struct _title title ) ;
int selectTitle ( int category_id, int subject_id, struct _title *title, char *show, char *sort, char *cursor ) ;

int readTitle ( char *string, struct _title *cur_title ) ;



int SelectInbox ( struct _user user, struct _title *title, char *show, char *sort, char *cursor ) ;


int selectSubject ( struct _subject *subject, char *show, char *sort, char *cursor ) ;
int insertSubject ( struct _subject *subject ) ;
int updateSubject ( struct _subject *subject ) ;
int deleteSubject ( struct _subject *subject ) ;


int selectLink ( int category_id, int subject_id, int object_id, struct _link *link ) ;
int deleteLink ( struct _link *link ) ;
int updateLink ( struct _link *link ) ;
int insertLink ( struct _link *link ) ;


int selectArticle ( int category_id, int subject_id, int object_id, struct _article *article ) ;
int deleteArticle ( struct _article *article ) ;
int updateArticle ( struct _article *article ) ;
int insertArticle ( struct _article *article ) ; 

int selectMessage ( int category_id, int subject_id, int object_id, struct _message *message ) ;
int deleteMessage ( struct _message *message ) ;
int updateMessage ( struct _message *message ) ;
int insertMessage ( struct _message *message ) ;

int getTitle ( int  category_id, int subject_id, int object_id, struct _title *title ) ;

/*   */

void searchIntra () ;

void groupMail ( struct _article *article ) ;



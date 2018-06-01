 /* funktioner för gränssnitt i HTML */


 /* these are common functions for several different programs */

void SubjectListBoxHTML ( struct _subject *subject ) ;
void SubjectTargetListBoxHTML ( struct _subject *subject ) ;

void CategoryListBoxHTML ( struct _category *category ) ;
void CategoryToListBoxHTML ( struct _category *category ) ;

void InformationClassListboxHTML ( int iInformationClass) ;
void InformationClassToListboxHTML ( int iInformationClass ) ;

void TextBoxHTML ( char *name, char *value, int size ) ;
void EditBoxHTML ( char *name, char *value, int rows, int cols ) ;

 /* functions used for navigation to a single object, and administration of categories and subjects */

void printCategoryHTML ( struct _user user, struct _category *category ) ;
void printCategoryFormHTML ( struct _category category ) ;

void printSubjectHTML ( struct _user  user,  struct _subject *subject ) ;
void printSubjectFormHTML ( struct _subject subject ) ;

void printTitleHTML ( int category_id, int subject_id, struct _user  user, struct _title *title ) ;
void printAllTitleHTML ( struct _user user, struct _title *title ) ;

void printObjectCategoryForm ( struct _category category, char *program, char *sLocale ) ;

int printInformationClassHTML ( int iInformationClass )  ;

 /*  functions for object article */

void printArticleHTML ( struct _user user, struct _article article ) ;
void printArticleForm ( struct _article article ) ;
void printArticleCategoryForm ( struct _category category ) ;

  /* functions for  message */

void printMessageHTML ( struct _user user, struct _message message ) ;
void printMessageForm ( struct _message message ) ;
void printMessageCategoryForm ( struct _category category ) ;


void printCommentForm ( struct _message message ) ;
void printCommentCategoryForm ( struct _category category ) ;

void printLinkHTML ( struct _link link ) ;
void printLinkForm ( struct _link link ) ; 
void printLinkCategoryForm ( struct _category category ) ;



/* 


  <a href="/cgi-bin/index.cgi?method=search?title=A">A</A>


  

 
*/


/* 


Om texten inneh�ller [[keyword]] s� 
  skall l�nk till sidan skapas enligt <a href=�/cgi-bin/article.cgi?method=search&title=keyword�>

*/

j = 0 ; i = 0 ;

while ( string[i] != NULL )
 {

  /* check if string contains [[ */
 
  if ( string[i] == �[� ) and ( string[i+1] == �[�)
   { 
    i = i +2 ; /* skip [[ */
       
    printf ("<a href=/"//cgi-bin/article.cgi?method=search&title=") ;

    /* get keyword */
  
    while ( string[i] != NULL )
     {

     /* if string contians ]] */

      if (( string[i] != � ]� ) and ( string[i+1] != �]�))
       { 
        i = i +2 ; /* skip ]] */   
        break ;
       }
        
      keyword[j] = string[i] ;
      i++ ; j ++ ;
 
     } 	/* end of while */
 
    keyword[j] = NULL ;

    printf ("%s/">%S</a>",keyword,keyword ) ;
                  
    } /* end of if */    

  if string[i] != NULL   
    printf("%c", string[i] ;

  i++ ;   /* next char */
    
 }  /* end of while */


/* 


f�r varje kategori
  f�r varje �mnesomr�de
    l�s en rubrik i taget
      om rubrik == keyword s� 
        funnen
      
*/





 





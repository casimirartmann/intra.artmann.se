



SelectBillboard ( struct _subject board[8][16] )

/* 

L�ser inl�gg fr�n fil och sorterar upp rubriker i ordning

kolumn	rad	kategori	�mne	id	

*/

{
    /* */

}   /* end of SelectBillboard () */

printBillboardHTML ( struct _subject board[8][16] ) 

{

  /* F�r varje kolumn */

for (i=0;i<cols;i++) 
 {
    /* f�r varje rad */
  for (j=0;j<rows;j++ )
   { 
    if ( strlen ( board[i][j].name) > 0 )  /* om rubrik finns */
     {
      printf (" ") ;               /* skriv ut l�nk-script */
    
     } 
   }
 }

}  /* end of printBillboardHTML () */

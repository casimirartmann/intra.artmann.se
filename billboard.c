



SelectBillboard ( struct _subject board[8][16] )

/* 

Läser inlägg från fil och sorterar upp rubriker i ordning

kolumn	rad	kategori	ämne	id	

*/

{
    /* */

}   /* end of SelectBillboard () */

printBillboardHTML ( struct _subject board[8][16] ) 

{

  /* För varje kolumn */

for (i=0;i<cols;i++) 
 {
    /* för varje rad */
  for (j=0;j<rows;j++ )
   { 
    if ( strlen ( board[i][j].name) > 0 )  /* om rubrik finns */
     {
      printf (" ") ;               /* skriv ut länk-script */
    
     } 
   }
 }

}  /* end of printBillboardHTML () */

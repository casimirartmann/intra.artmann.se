sendmail ( char *from, char *to, char *cc, char *bcc, char *subject, char *text ) 

  /* 
     from     - avs�ndare
     to       - mottagare
     cc       - kopia till
     bcc      - dold kopia
     subject  - rubrik
     text     - text
  */
{ 

char string[255] ;
                 
                 
/* V�lj vilken av f�ljande tre rader med sprintf som skall anv�ndas */

/*   denna kod anv�nds f�r UNIX-datorer med sendmail */
/* sprintf ( string, "sendmail %s < e-mail.txt", sendto ) ;   */

/*   denna kod anv�nds f�r UNIX-datorer med mailx */
/* sprintf ( string, "mailx -r%s %s < e-mail.txt", from, sendto ) ; */

/*   denna kod anv�nds f�r NT-system med blat */
/* sprintf ( string, "blat e-mail.txt -s %s -t %s", subject, sendto ) ; */

/* system ( string ) ;  
  */
  
return ( TRUE ) ;
}

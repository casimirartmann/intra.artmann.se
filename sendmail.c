sendmail ( char *from, char *to, char *cc, char *bcc, char *subject, char *text ) 

  /* 
     from     - avsändare
     to       - mottagare
     cc       - kopia till
     bcc      - dold kopia
     subject  - rubrik
     text     - text
  */
{ 

char string[255] ;
                 
                 
/* Välj vilken av följande tre rader med sprintf som skall användas */

/*   denna kod används för UNIX-datorer med sendmail */
/* sprintf ( string, "sendmail %s < e-mail.txt", sendto ) ;   */

/*   denna kod används för UNIX-datorer med mailx */
/* sprintf ( string, "mailx -r%s %s < e-mail.txt", from, sendto ) ; */

/*   denna kod används för NT-system med blat */
/* sprintf ( string, "blat e-mail.txt -s %s -t %s", subject, sendto ) ; */

/* system ( string ) ;  
  */
  
return ( TRUE ) ;
}

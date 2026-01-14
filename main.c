#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

#define MAX_LENGTH 1024
#define DELIMS " \t\r\n"

//mesaj de help
char help_msg[] = 
"Comenzi valabile:\n"
"  mv <source> <destination>\n"
"  cp <source> <destination>\n"
"  rm <file>\n"
"  mkdir <directory>\n"
"  ls [directory]\n"
"  help\n";

// verifica daca path-ul este de tip dropbox
int isDBX(char *path)
{
    if(strstr(path, "dbx:") == path)
    {
        return 1;
    }
    return 0;
}

// functie mkdir simpla
int mkdir_simple(char *path)
{
    if(!path || !*path)
       return -1;
    if(mkdir(path, 0755) == -1)
    {
        perror("mkdir");
        return -1;
    }
    return 0;
}
int main(int argc, char *argv[]) {
    char line[MAX_LENGTH];
    char *p;
    while(1)
    {
        printf("$ ");
        if (fgets(line, MAX_LENGTH, stdin) == NULL) { // CTR+D to exit
            break;
        }
        if(strcmp(line, "exit\n") == 0) {
            break;
        }
        if(strchr(line,'\n')!=NULL)
        {
            line[strlen(line)-1] = '\0'; 
        }

        p = strtok(line, DELIMS);
        if(p != NULL)
        {
            if(strcmp(p,"mv")==0)
            {
                //TODO: Implement mv
            }
            if(strcmp(p,"cp")==0)
            {
                //TODO: Implement cp
            }
            if(strcmp(p,"rm")==0)
            {
                //TODO: Implement rm
            }
            if(strcmp(p,"mkdir")==0)
            {
                //TODO: Implement mkdir
            }
            if(strcmp(p,"ls")==0)
            {
                //TODO: Implement ls
            }
            if(strcmp(p,"help")==0)
            {
                printf("%s", help_msg);
            }
        }
        printf("%s\n", line);
    }
    return 0;
}
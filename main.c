#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#define MAX_LENGTH 1024
#define DELIMS " \t\r\n"

char *src,*dst, *com;

//mesaj de help
char help_msg[] = 
"Comenzi valabile:\n"
"  mv <source> <destination>\n"		
"  cp <source> <destination>\n"		
"  rm <file>\n"						
"  mkdir <directory>\n"				
"  ls [directory]\n"				
"  help\n"
"  Interactiunea cu DropBox se face cu prefixul dbx:[target]\n"
"  Ex: mv foo.txt dbx:bar/baz.txt\n"	;							


// verifica daca path-ul este de tip dropbox
int isDBX(char *path)
{
    if(strstr(path, "dbx:") == path)
    {
    	return 1;
    }
    return 0;
}
//functii dbx
int mkdir_dbx(char *path)
{
    pid_t pid=fork();
    if(pid == -1)
        return -1;
    if(pid == 0)
    {
        execlp("dbxcli", "dbxcli", "mkdir", path, NULL);
        perror("execlp");
    }
    else
    {
        wait(NULL);
    }
    return 0;
}
int ls_dbx(char *path){
    pid_t pid = fork();
    if (pid == 1)
        return -1;
    if(pid == 0){
        execlp("dbxcli", "dbxcli", "ls", path, NULL);
        perror("execlp");
    }
    else{
        wait(NULL);
    }
}
int rm_dbx(char *path){
    pid_t pid = fork();
    if (pid == 1)
        return -1;
    if(pid == 0){
        execlp("dbxcli", "dbxcli", "rm", path, NULL);
        perror("execlp");
    }
    else{
        wait(NULL);
    }
}

int mv_dbx(char *src,char *dst)
{
    pid_t pid=fork();
    if(pid == -1)
        return -1;
    if(pid == 0)
    {
        execlp("dbxcli", "dbxcli", "mv", src, dst, NULL);
        perror("execlp");
    }
    else
    {
        wait(NULL);
    }
    return 0;
}
int cp_dbx(char *src,char *dst)
{
    pid_t pid=fork();
    if(pid == -1)
        return -1;
    if(pid == 0)
    {
        execlp("dbxcli", "dbxcli", "mv", src, dst, NULL);
        perror("execlp");
    }
    else
    {
        wait(NULL);
    }
    return 0;
}
int up_dbx(char *src, char *dst){
	pid_t pid = fork();
	if (pid == -1)
		return -1;
	if (pid == 0){
        int fd = open("/dev/null", O_WRONLY);
        if(fd != -1){
            dup2(fd, STDOUT_FILENO);
            dup2(fd, STDERR_FILENO);
            close(fd);
        }
		execlp("dbxcli", "dbxcli", "put", src, dst, NULL);
		perror("execlp");
	}
	else{
		wait(NULL);
	}
	return 0;	
}

int down_dbx(char *src, char *dst)
{
	pid_t pid = fork();
	if(pid == -1)
		return -1;
	if(pid == 0)
	{
        int fd = open("/dev/null", O_WRONLY);
        if(fd != -1){
            dup2(fd, STDOUT_FILENO);
            dup2(fd, STDERR_FILENO);
            close(fd);
        }
		execlp("dbxcli","dbxcli","get",src, dst, NULL);
		perror("execlp");
	}
	else
	{
		wait(NULL);
	}
}
// functii locale
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
int ls_simple(char *path){
	if (path == NULL) path = ".";
	pid_t pid = fork();
	
	if (pid == -1){
		perror("fork");
		return -1;
	}
	if (pid == 0){
		execv("/bin/ls", (char *[]){"ls", path, NULL});
		exit(1);
	}
	else wait(NULL);
	return 0;
}

int rm_simple(char *path)
{
	if(!path || !*path)
       return -1;
	struct stat st;
    if (lstat(path, &st) == -1)
    {
        perror("rm");
        return -1;
    }
    if (S_ISDIR(st.st_mode))
    {
        if (rmdir(path) == -1)
        {
            perror("rmdir");
            return -1;
        }
        return 0;
    }
    if (unlink(path) == -1)
    {
        perror("unlink");
        return -1;
    }

    return 0;
}
int mv_simple(char *src, char *dst){
	pid_t pid = fork();
	
	if (pid == -1){
		perror("fork");
		return -1;
	}
	
	if (pid == 0){
		execlp("mv", "mv", src, dst, NULL);
		perror("execlp mv");
		exit(1);
	}
	else{
		wait(NULL);
	}
	return 0;
}
int cp_simple(char *src, char *dst){
	pid_t pid = fork();
	if (pid == -1){
		perror("fork");
		return -1;
	}
	if (pid == 0){
		execv("/bin/cp", (char *[]){"cp", src, dst, NULL});
		perror("execv cp");
		exit(1);
	}
	else
		wait(NULL);
	return 0;
}
void parsare(char *line){
    int unu=0, doi=0;
    int c = 0;
    int com_counter = 0;
    char *buffer = malloc(1024);
    while(strchr(DELIMS, line[c]) != NULL){
        c++;
    }
        while(strchr(DELIMS, line[c]) == NULL){
        com[com_counter++]=line[c];
        c++;
    }
    com[com_counter]='\0';

    if ((strstr(com,"ls") && strlen(line) > 3) || strstr(com, "mkdir") || strstr(com, "rm")) unu = 1;
    if (strstr(com,"mv") || strstr(com,"cp")) unu = doi = 1;
    if (unu){
        line = line+c+1;
        c = 0;
        if (strstr(line, "dbx:\"")){// mv dbx:"daniel" dbx:"dan"
            c = strstr(line, "dbx:\"") - line;
            strncpy(buffer, line, c);
            buffer[c]='\0';
            strcat(buffer, "\"dbx:");
            strcat(buffer, line + c + 5);
            c = 0;
            strcpy(line, buffer);
        }
        free(buffer);
        if (line[0]=='"'){
            line+=1;
            c=0;
            while(line[c]!='"'){
                src[c]=line[c];
                c++;
            }
        }
        else{
            c=0;
            while(line[c]!=' '){
                src[c]=line[c];
                c++;
            }
        }
        line = line + c + 1;
        src[c]='\0';
    }
    if (doi){
        char *buffer2 = malloc(1024);
        if (strstr(line, "dbx:\"")){// mv dbx:"daniel" dbx:"dan"
            line+=1;
            c = strstr(line, "dbx:\"") - line;
            strncpy(buffer2, line, c);
            buffer2[c]='\0';
            strcat(buffer2, "\"dbx:");
            strcat(buffer2, line + c + 5);
            c = 0;
            strcpy(line, buffer2);
        }
        if(line[0]==' ')line++;
        free(buffer2);
        if (line[0]=='"'){
            line+=1;
            c=0;
            while(line[c]!='"'){
                dst[c]=line[c];
                c++;
            }
        }
        else{
            c=0;
            if (line[c]==' ')line++;
            while(strchr(DELIMS, line[c]) == NULL && line[c]!='\0'){
                dst[c]=line[c];
                c++;
            }
        }
        dst[c]='\0';
    }
}
//ulei
int main(int argc, char *argv[]) {

	printf("%s", help_msg);
    char line[MAX_LENGTH];
    char *p;
    while(1)
    {
		com = malloc(10);
    	src = malloc(256);
    	dst = malloc(256);
        com[0] = '\0';
        src[0] = '\0';
        dst[0] = '\0';
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

        parsare(line);

        if(com != NULL)
        {
            if(strcmp(com,"mv")==0)
            {

				if(strlen(src) == 0 || strlen(dst) == 0)
				{
					printf("usage: mv <source> <destination>");
				}
				else
				{
					if(isDBX(src))
					{
						if(isDBX(dst))
						{
							mv_dbx(src+4,dst+4);
						}
						else
						{
							down_dbx(src+4,dst);
							rm_dbx(src+4);
						}

					}
                    else{
                        if(isDBX(dst))
                        {
                            up_dbx(src,dst+4);
							rm_simple(src);
                        }
                        else
                        {
                            mv_simple(src,dst);
                        }
                    }
				}
            }
            else if(strcmp(com,"cp")==0)
            {

				if(strlen(src) == 0 || strlen(dst) == 0)
				{
					printf("usage: cp <source> <destination>");
				}
				else
				{
					if(isDBX(src))
					{
						if(isDBX(dst))
						{
							cp_dbx(src+4,dst+4);
						}
						else
						{
							down_dbx(src+4,dst);
						}

					}
                    else{
                        if(isDBX(dst))
                        {
                            up_dbx(src,dst+4);
                        }
                        else
                        {
                            cp_simple(src,dst);
                        }
                    }
				}
            }
            else if(strcmp(com,"rm")==0)
            {

				if(strlen(src) == 0)
				{
					printf("rm: nu are argument\n");
				}
				else
				{
					if(isDBX(src))
					{
						rm_dbx(src+4);
						
					}
					else{
						rm_simple(src);
					}
				}
            }
            else if(strcmp(com,"mkdir")==0)
            {
 
                if(strlen(src) == 0)
                {
                    printf("mkdir: nu are argument\n");
                }
                else
                {
                    if(isDBX(src))
                    {
                        mkdir_dbx(src+4);
                    }
                    else
                    {
                        mkdir_simple(src);
                    }
                }
            }
            else if(strcmp(com,"ls")==0){

				if(strlen(src) == 0){
                    char *path = malloc(2);
                    path[0]='.';
                    path[1]='\0';
					ls_simple(path);
                    free(path);
                    
				}
				else{
					if (isDBX(src)){
						ls_dbx(src+4);
                        printf("\n");
                    }
					else
						ls_simple(src);	
				}
            }
            else if(strcmp(com,"help")==0)
            {
                printf("%s", help_msg);
            }
        }
		free(src);
    	free(dst);
    	free(com);
    }

    return 0;
}
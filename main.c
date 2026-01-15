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

char *src,*dst;

//mesaj de help
char help_msg[] = 
"Comenzi valabile:\n"
"  mv <source> <destination>\n"		//50%
"  cp <source> <destination>\n"		//merge 100%
"  rm <file>\n"						//merge 100%
"  mkdir <directory>\n"				//merge 100%
"  ls [directory]\n"				//merge 100%
"  help\n";							//idk 


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
//ulei
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
                p = strtok(NULL,DELIMS);
				src = p;
				p = strtok(NULL,DELIMS);
				dst = p;
				if(src==NULL || dst == NULL)
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
            else if(strcmp(p,"cp")==0)
            {
                p = strtok(NULL,DELIMS);
				src = p;
				p = strtok(NULL,DELIMS);
				dst = p;
				if(src==NULL || dst == NULL)
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
            else if(strcmp(p,"rm")==0)
            {
                p = strtok(NULL,DELIMS);
				if(p == NULL)
				{
					printf("rm: nu are argument\n");
				}
				else
				{
					if(isDBX(p))
					{
						rm_dbx(p+4);
						
					}
					else{
						rm_simple(p);
					}
				}
            }
            else if(strcmp(p,"mkdir")==0)
            {
                p = strtok(NULL, DELIMS);
                if(p == NULL)
                {
                    printf("mkdir: nu are argument\n");
                }
                else
                {
                    if(isDBX(p))
                    {
                        mkdir_dbx(p+4);
                    }
                    else
                    {
                        mkdir_simple(p);
                    }
                }
            }
            else if(strcmp(p,"ls")==0){
                p = strtok(NULL, DELIMS);
				if(p == NULL){
					ls_simple(p);
                    
				}
				else{
					if (isDBX(p)){
						ls_dbx(p+4);
                        printf("\n");
                    }
					else
						ls_simple(p);	
				}
            }
            else if(strcmp(p,"help")==0)
            {
                printf("%s", help_msg);
            }
        }
    }
    return 0;
}
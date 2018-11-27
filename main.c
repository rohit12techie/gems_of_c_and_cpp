#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define SHELL_NAME "rshell:~$"

const char *rshellcmd[4] = {"help", "exit", "about", "version"};

/*Help function*/
void printhelp()
{
	printf("rshell version 1.0\n");
	printf("these are the few internal commands for rshell\n\n");
	printf("\thelp- \n\tabout-\n\tversion-\n\texit-\n");
}

/*Existing the rhsell*/
void exitrshell()
{
	printf("exiting rshell\n");
	exit(0);
}

/*About rshell*/
void aboutrshell()
{
	printf("rhell version 1.0\n");
	printf("Developed by Rohit Kumar [rohit12techie@gmail.com]\n");
	printf("copyright: Rohit Kumar\n");
}

/*Version of rshell*/
void versionrshell()
{
	printf("rshell version 1.0\n");
}

/*Handling internal command to rshell like "help, about, version, exit"*/
int handleinternal(char *cmd)
{
	for(int i=0; i<4; i++){
		if(strcmp(cmd,rshellcmd[i]) == 0){
			switch(i){
				case 0: printhelp(); break;
				case 1: exitrshell(); break;
				case 2: aboutrshell(); break;
				case 3: versionrshell();break;
			}
			return 1;
		}
	}

	return 0;
}

int main()
{
	printf("#########################################################################\n");
	printf("##                                                                     ##\n");
	printf("##                        Welcome to rshell                            ##\n");
	printf("##                                                                     ##\n");
	printf("#########################################################################\n");
	
	while(1){
		printf(SHELL_NAME);
		char *str = NULL;
		size_t size = 0;
		int i =0;
		char *strlist[4];
		
		int len = getline(&str, &size, stdin);
		str[len-1]='\0';
	
		if(handleinternal(str)){
			continue;
		}

		char *tok = strtok(str," ");
		while(NULL != tok){
			strlist[i++] = tok;
			tok = strtok(NULL," ");
		}
		strlist[i]=NULL;
	
		pid_t pid = fork();
		if(pid >= 0){
			if(pid == 0){
				if (execvp(strlist[0],strlist) < 0) {
					printf("\n%s: command not found\n",strlist[0]);
				}
				exit(0);
			}	
			else{
				wait(NULL);
			}	
		}
		else{
			printf("fork() failed\n");
		}
	}
	return 0;
}

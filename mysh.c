#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "mysh.h"

/**
 * Execute other commands.
 */
void execOtherCommands(char *command, char **argv, int *list, int* states_list)
{


    if(!strcmp(command, "exit"))
	{
		
		
	}
	
    if (execvp(command, argv))
    {
     writeError(1);
    }
    exit(1);
	
}

int main(int inargc, char *inargv[])
{
    if (inargc > 1)
    {
        writeError(1);
    }

    // store input
    char *inputBuffer = malloc(131 * sizeof(char));
    char *miscellaneousBuffer = malloc(1025 * sizeof(char));
    // count of numbers of commands
    int commandCount = 0;

    int continueLooping = 1;
     int *list;
     list = malloc(20 *sizeof(int));
 	for (int i = 0; i < 20; i++){
		*(list+i) = -1;
	}

	 int * states_list;
     states_list = malloc(20 *sizeof(int));
 	for (int i = 0; i < 20; i++){
		*( states_list+i) = -1;
	}
	
    // shell loop
    while (continueLooping)
    {

	for(int i = 0; i < 20; i++)
		{
			//printf("%d\n",*(list +i) );
			if(*(list +i)!= -1)
			{	
					
				//printf("%d\n+kill here + %p\n", waitpid(*(list+i),(states_list +i) ,WNOHANG), (states_list +i));
				
				if(waitpid(*(list+i),(states_list+i),WNOHANG) > 0){
						
						*(list+i) == -1;
					
						}
		}
	}



	//for(int i = 0; i < 20; i++){
	//if(*(list+1)!= -1){
		//if(waitpid(*(list+i),0,WNOHANG) > 0){
		//	*(list+i) == -1;
	//	}
		
	//}
	//}

	
	
	
        // print out shell prompt mysh (\!) >
        sprintf(miscellaneousBuffer, "mysh (%d)> ", ++commandCount);
        if (write(STDOUT_FILENO, miscellaneousBuffer, strlen(miscellaneousBuffer)) == -1)
        {
            writeError(1);
        }

        // read input
        if (!fgets(inputBuffer, 130, stdin))
        {
            writeError(1);
        }

        // determine if input line is too long
        if (strlen(inputBuffer) >= 129 && inputBuffer[128] != '\n')
        {
            flushLine(inputBuffer);
            continue;
        }
        // process input buffer
        strip(inputBuffer);
        // count args:
        int argc = countArgs(inputBuffer);
        // allocate arg vector + NULL entry at end
        char *argv[argc + 1];
        // get arguments
        getArgs(argc, argv, inputBuffer);
	
        int runInBackgroundFlag = runInBackground(&argc, argv);
	//check all space input
	char *test = inputBuffer; 
        int empty = 1;
       
        while (test <= test+ strlen(inputBuffer)-1)
        {
                if(*test!='\n') {
                empty = 0;
                break;

                }++test;  }
        // if no arguments or input buffer of no length
        if (argc == 0 || strlen(inputBuffer) == 0)
        {
            commandCount--;
        }
	else if(empty == 1){
		continue;
		//?
	}
        else if (!strcmp(argv[0], "exit"))
        {

		for(int i = 0; i < 20; i++)
		{
			//printf("%d\n",*(list +i) );
			if(*(list +i)!= -1)
			{	
				
				kill(*(list+i), SIGKILL);

			}
		}
            continueLooping = 0;
        }
        // cd
        else if (!strcmp(argv[0], "cd"))
        {
            myCd(argv[1]);
        }
        // execute the other commands
        else
        {
            // fork
            int childPid = fork();
            if (childPid == 0)
            {
                int pipeIndex = hasPipeOperator(argc, argv);

                // if no pipe
                if (pipeIndex == -1)
                {
                    // find output and input files
                    setIOFds(&argc, argv);
                    // handle pwd
                    if (!strcmp(argv[0], "pwd"))
                    {
                        myPwd(argc, miscellaneousBuffer);
                    }
                    // handle commands not implemented by mysh
                    else
                    {
                        execOtherCommands(argv[0], argv, list,  states_list);
                    }
                }
                // there is a pipe
                else
                {
                    // arg for each process
                    int argc2 = argc - (pipeIndex + 1);
                    int argcv[] = {argc, argc2};
                    argv[pipeIndex] = NULL;

                    if (argc2 == 0 || argc == 0)
                    {
                        writeError(1);
                    }

                    // args for each process
                    char **argvv[2] = {argv, argv + pipeIndex + 1};

                    // ends of pipe
                    int pipeFds[2];
                    int pipeReturn = pipe(pipeFds);
                    if (pipeReturn)
                    {
                        writeError(1);
                    }

                    // argc and argv for process, to be set
                    int thisArgc = 0;
                    char **thisArgv;
                    int childChildPid = -1;
                    // fork
                    childChildPid = fork();
                    // child
                    if (childChildPid != 0)
                    {
                        thisArgc = argcv[1];
                        thisArgv = argvv[1];
                        dup2(pipeFds[0], STDIN_FILENO);
                        close(pipeFds[1]);
                    }
                    else if (childChildPid == 0)
                    {
                        thisArgc = argcv[0];
                        thisArgv = argvv[0];
                        dup2(pipeFds[1], STDOUT_FILENO);
                        close(pipeFds[0]);
                    }
                    else
                    {
                        writeError(1);
                    }

                    if (!strcmp(thisArgv[0], "pwd"))
                    {
                        myPwd(thisArgc, miscellaneousBuffer);
                    }
                    // handle commands not implemented by mysh
                    else
                    {
                        execOtherCommands(thisArgv[0], thisArgv, list, states_list);
                    }
                    exit(0);
                }
            }
            else if (childPid == -1)
            {
                writeError(1);
            }
            else
            {
                if (!runInBackgroundFlag)
                {
                    // wait for child process to exit
                    int status = 0;
                    while (waitpid(childPid, &status, 0) != childPid)
                        ;
		   
                }
		else{

			
			for(int i = 0; i < 20; i++)
			{
				if(*(list+i) == -1) {
					*(list+i) = childPid;
					*(states_list + i) =0;
					break;}
			}  
		}

            }
	//	int status = 0;
	//	 while((childPid = waitpid(-1, &status, WNOHANG)) > 0);
        }
    }

    free(inputBuffer);
    free(miscellaneousBuffer);
    free(list);
    free(states_list);

}

/**
 * Remove newline, leading, trailing, and duplicate spaces in place.
 */
void strip(char *string)
{
    // remove leading spaces
    char *firstNonSpace = string;
    // find first nonspace
    while (*firstNonSpace != '\0' && isspace(*firstNonSpace))
    {
        ++firstNonSpace;
    }
    size_t len = strlen(firstNonSpace);
    // shift everything past the first nonspace down
    memmove(string, firstNonSpace, len);

    // remove trailing spaces
    char *endOfString = string + len;
    while (string < endOfString && (isspace(*endOfString) || (*endOfString == '\n')))
    {
        --endOfString;
    }
    // terminate string
    *(endOfString) = '\0';

    // remove duplicate spaces
    char *back = string;
    char *front = string + 1;
    while (*front != '\0')
    {
        if (!(isspace(*front)) ||  !(isspace(*back)))
        {
            *(++back) = *front;
        }
        front++;
    }
    // terminate string
    *(back) = '\0';
}

/**
 * Returns the number of arguments in the string, determined by the spaces.
 */
int countArgs(char *string)
{
    if (!strlen(string))
    {
        return 0;
    }
    int count = 0;
    char *ptr = string;
    while ((ptr = strchr(ptr, ' ')) != NULL)
    {
        count++;
        ptr++;
    }
    return count + 1;
}

/**
 * Gets the arguments from the input.
 */
void getArgs(int argc, char *argv[], char *inputBuffer)
{
    // read in arguments using strtok_r
    char *saveptr = NULL;
    argv[0] = strtok_r(inputBuffer, "\t\n ", &saveptr);
    for (int i = 1; i < argc; ++i)
    {
        argv[i] = strtok_r(NULL, " \t\n", &saveptr);
    }
    // set final element of argv to null
    argv[argc] = NULL;
}

/**
 * Shift arguments after a certain index down by some amount.
 */
void shiftStringsDown(char *argv[], int *argc, int start, int shiftAmount)
{
    // shift the arguments past 'start' down
    for (int i = start; i < *argc - shiftAmount; ++i)
    {
        argv[i] = argv[i + shiftAmount];
    }
    // clear end of array
    for (int i = 0; i < shiftAmount; ++i)
    {
        argv[*argc - 1 - i] = NULL;
    }
    // change the number of args
    *argc -= shiftAmount;
    // set last element of the array to be null, as required by execvp
    argv[*argc] = NULL;
}

/**
 * Set input and output file descriptors. 
 */
void setIOFds(int *argc, char *argv[])
{
    for (int i = 0; i < *argc; ++i)
    {
        // if output redirected
        if (!strcmp(argv[i], ">"))
        {
            // there are no more arguments but > specified
            if (i >= *argc - 1)
            {
                writeError(1);
            }
            // set output fd
            int outFd = open(argv[i + 1], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
            if (outFd == -1)
            {
                writeError(1);
            }
            dup2(outFd, STDOUT_FILENO);
            // if there are any more arguments, they must be for input redirection
            if (i + 2 < *argc && strcmp(argv[i + 2], "<"))
            {
                writeError(1);
            }
            // remove output redirection from arguments
            shiftStringsDown(argv, argc, i--, 2);
        }
        // if input redirected
        else if (!strcmp(argv[i], "<"))
        {
            // there are no more arguments but < specified
            if (i >= *argc - 1)
            {
                writeError(1);
            }
            // set input fd
            int inFd = open(argv[i + 1], O_RDONLY);
            if (inFd == -1)
            {
                writeError(1);
            }
            dup2(inFd, STDIN_FILENO);
            // if there are any more arguments, they must be for output redirection
            if (i + 2 < *argc && strcmp(argv[i + 2], ">"))
            {
                writeError(1);
            }
            // remove input redirection from arguments
            shiftStringsDown(argv, argc, i--, 2);
        }
    }
}

/**
 * Write out the error message, and exit if requested.
 */
void writeError(int exitStatus)
{
    // write error message
    if (write(STDERR_FILENO, error_message, strlen(error_message)) == -1)
    {
        exit(1);
    }

    // if exit status is nonzero, exit with that status
    if (exitStatus)
    {
        exit(exitStatus);
    }
}

void flushLine(char *inputBuffer)
{
    // if so, flush the line out of the buffer
    int flushLongLine = 1;
    while (flushLongLine)
    {
        if (!fgets(inputBuffer, 2, stdin))
        {
            writeError(1);
        }
        else if (inputBuffer[0] == '\n')
        {
            flushLongLine = 0;
        }
    }
    writeError(0);
}

/**
 * My cd function.
 */
void myCd(char *dir)
{
    // if path specified, cd there
    if (dir)
    {
        if (chdir(dir))
        {
            writeError(0);
        }
    }
    // else cd home
    else
    {
        if (chdir(getenv("HOME")))
        {
            writeError(0);
        }
    }
}

/**
 * My pwd function.
 */
void myPwd(int argc, char *miscellaneousBuffer)
{
    // only one arg (pwd) can be specified for pwd
    if (argc != 1)
    {
        writeError(1);
    }

    if (!getcwd(miscellaneousBuffer, 1024))
    {
        writeError(1);
    }
    // append new line
    miscellaneousBuffer[strlen(miscellaneousBuffer)] = '\n';
    if (write(STDOUT_FILENO, miscellaneousBuffer, strlen(miscellaneousBuffer)) == -1)
    {
        writeError(1);
    }
    exit(0);
}



/**
 * Returns 1 if the command is to be run in the background.
 */
int runInBackground(int *argc, char **argv)
{

	
    for (int i = 0; i < *argc; i++)
    {
	//printf("%s\n~~~~",argv[i]);
        // if an arg is '&' then run in the background
        if (!strcmp(argv[i], "&"))
        {
		//printf("%s\n -----------------",argv[i]);
            shiftStringsDown(argv, argc, i, 1);
            return 1;
      
    }
   
	}
 return 0;
}

/**
 * Returns index of pipe operator if exists, or -1.
 */
int hasPipeOperator(int argc, char **argv)
{
    for (int i = 0; i < argc; i++)
    {
        if (!strcmp(argv[i], "|"))
        {
            return i;
        }
    }
    return -1;
}

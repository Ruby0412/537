#ifndef _MYSH_H_
#define _MYSH_H_

// data
char error_message[30] = "An error has occurred\n";

// functions
void strip(char *string);
int countArgs(char *string);
void getArgs(int argc, char *argv[], char *inputBuffer);
void shiftStringsDown(char *argv[], int *argc, int start, int num);
void setIOFds(int *argc, char *argv[]);
void writeError(int exitStatus);
void flushLine(char *inputBuffer);
void myCd(char *dir);
void myPwd(int argc, char *miscellaneousBuffer);
void execOtherCommands(char *command, char **argv, int *list, int *status_list);
int runInBackground(int *argc, char ** argv);
int hasPipeOperator(int argc, char **argv);

#endif

/**
 *version control and author 
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>


#define MAX_LINE		80 /* 80 chars per line, per command, should be enough. */
#define MAX_COMMANDS	9 /* size of history */

char history[MAX_COMMANDS][MAX_LINE]; //the array used to store history commands.
char display_history [MAX_COMMANDS][MAX_LINE]; 
/*the array used for "printf" to display history nicely. Remove special characters like "\n" or "\0"*/

int command_count = 0;

/**
 * Add the most recent command to the history.
 */

void addtohistory(char inputBuffer[]) {

	int i = 0;
	// update array"history": add the command to history, strcpy(str1,str2);
	strcpy(history[(command_count - 1) % MAX_COMMANDS], inputBuffer);
	
	// update array"display_history": remove characters like '\n', '\0' in order to display nicely
	while (inputBuffer[i] != '\n' && inputBuffer[i] != '\0'){
		display_history[(command_count - 1) % MAX_COMMANDS][i] = inputBuffer[i];
		i++;
	}
	display_history[(command_count - 1) % MAX_COMMANDS][i] = '\0';
	return;
}

/** 
 * The setup function below will not return any value, but it will just: read
 * in the next command line; separate it into distinct arguments (using blanks as
 * delimiters), and set the args array entries to point to the beginning of what
 * will become null-terminated, C-style strings. 
 */

int setup(char inputBuffer[], char *args[],int *background)
{
    int length,		/* # of characters in the command line */
	i,				/* loop index for accessing inputBuffer array */
	command_number,	/* index of requested command number */
	beg,			// index of beginning of a string
	ct_arg = 0;	// arg to add string to

	bool isNewString = true;

	//define your local varialbes here;
	
	
    /* read what the user enters on the command line */
	do {
		printf("osh> ");
		fflush(stdout);
		length = read(STDIN_FILENO,inputBuffer,MAX_LINE); 
	}
	while (inputBuffer[0] == '\n'); /* swallow newline characters */


    if (length == 0)
        exit(0);            /* ^d was entered, end of user command stream */
 
    if ( (length < 0) ) {
		perror("error reading the command");
		exit(-1);           /* terminate with error code of -1 */
    }
	
	/**
	 * Check if they are using history
	 */
	
	// fill in your code here Part II, if the user input is to repeat some history command
	if (inputBuffer[0] == '!') {
		if (command_count == 0) {
				printf("No commands in history\n");
				return -1;
		}
		if (inputBuffer[1] == '!') {
			strcpy(inputBuffer, history[(command_count - 1) % MAX_COMMANDS]);
		}
		else {
			int num = atoi(&inputBuffer[1]);
			int max;
			if (command_count > MAX_COMMANDS)
				max = MAX_COMMANDS;
			else
				max = command_count;
			if (num < 1 || num > max) {
				printf("No such command in history\n");
				return -1;
			}
			strcpy(inputBuffer, history[num - 1]);
		}
	}
	length = strlen(inputBuffer);
	command_count++;

	//Add the command to the history
	addtohistory(inputBuffer); 
	
	//Parse the contents of inputBuffer
	
    for (i=0;i<length;i++) { 
		/* examine every character in the inputBuffer */
		
        switch (inputBuffer[i]){
			case ' ':
			case '\t' : /* argument separators */
				//set up args
				inputBuffer[i] = '\0';
				if (!isNewString) {
					args[ct_arg] = &inputBuffer[beg];
					isNewString = true;
					ct_arg++;	
				}	
				break;	
				
			case '\n':  /* should be the final char examined */
				//fill in your code here, set up the last item args[x] = NULL;
				/* no more arguments to this command */	
				inputBuffer[i] = '\0';
				if(!isNewString){
					args[ct_arg] = &inputBuffer[beg];
					ct_arg++;
				}
				break;	
	    	default :  /* some other character */
				//fill in your code here, 
				/* args[i] is a pointer to a string, its value is the address of the first charater of that string
				* You want to track the location of the beginning character of each string. 
				* The location is the first character, which is not '\t' and not '\n'
				* You also need check "&". If '&' is detected, setup background flag.
				*/  
				
				if (isNewString){
					if (inputBuffer[i] == '&') {
						*background = 1;
						break;
					}
					beg = i;
					isNewString = false;
				}
		}
		/* end of switch */
	}    /* end of for */
	args[ct_arg] = NULL;
	/**
	 * Here you finish parsing the input. 
	 * There is one more thing to assure. If we get '&', make sure you don't enter it in the args array
	*/
	
	return 1;
	
} /* end of setup routine */


int main(void)
{
	char inputBuffer[MAX_LINE]; 	/* buffer to hold the command entered */
	int background = 0;             /* equals 1 if a command is followed by '&' */
	char *args[MAX_LINE/2 + 1];	/* command line (of 80) has max of 40 arguments */
	pid_t child;            		/* process id of the child process */
	
	//define your local variables here, at the beginning of your program. 

	int shouldrun = 1;
	

		
    while (shouldrun){            		/* Program terminates normally inside setup */
		background = 0;
		
		shouldrun = setup(inputBuffer,args,&background);       /* get next command */
		
		// fill in your code here Part I
		/* if the user typed in "exit", the shell program will return (or terminate). 
		* Call strncmp(str1,str1,count). The function call will return 0 if str1 == str2.
		* "count" is the number of characters we use to compare.    
		*/		
		if (strncmp(inputBuffer, "exit", 4) == 0)
			return 0;
		
		// fill in your code here Part II
		/* if the user typed in "history", the shell program will display the history commands. 
		* you will use "printf" to print the display_history
		* after you display all the array, this command is done. 
		* Your program should go to read again, which means calling the "setup" function.  
		*/
		else if (strncmp(inputBuffer, "history", 7) == 0){
			int max;
			if (command_count < MAX_COMMANDS)
				max = command_count;
			else
				max = MAX_COMMANDS;
			for (int i = max; i > 0; i--)
				printf("%d\t%s\n", i, display_history[i - 1]);
			continue;
		}
		if (shouldrun) {			
			//creates a duplicate process!
			child = fork();
			//pid<0  error
			if(child < 0){
				printf("Error running command!!!\nExiting kernel\n");
				return -1;
			}
			//pid == 0, it is the child process. use the system call execvp(args[0],args);
			else if (child == 0){
				execvp(args[0], args);
			}
			//pid > 0, it is the parent. Here you need consider it is foreground or background
			else{
				if(!background)
					while (child != wait(NULL));	
			}
			
		}
	}
	
	return 0;
}

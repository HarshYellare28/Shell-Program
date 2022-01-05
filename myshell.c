#include <stdio.h>

#include <string.h>

#include <stdlib.h>			// exit()

#include <unistd.h>			// fork(), getpid(), exec()

#include <sys/wait.h>		// wait()

#include <signal.h>			// signal()

#include <fcntl.h>			// close(), open()



// void sigintHandler(int sigVal)

// {

// 	printf("Caught ^C\n");

// }



// void sigtstpHandler(int sigVal)

// {

// 	printf("Caught ^Z\n");

// }



char parseInput(char* inputLine, char* myArgs[],size_t size)

{

	// This function will parse the input string into multiple commands or a single command with arguments depending on the delimiter (&&, ##, >, or spaces).

	char* inputLineCopy = strdup(inputLine);

	size_t i = 0;

	char* currentArg;

	char delim = '0';

	

	// If user has just clicked enter without writing any command

	if(strlen(inputLine) == 0)

	{

		myArgs[0] = NULL;

		return delim;

	}



	// //special gateway for user input as exit 

	// if (inputLineCopy[0] == 'e' && inputLineCopy[1] == 'x' && inputLineCopy[2] == 'i' && inputLineCopy[3] == 't')

	// {

	// 	myArgs[0] = "exit";

	// 	int k = 1;

	// 	for(k=1;k<size;k++)

	// 	{

	// 		myArgs[k] == NULL;

	// 	}

	// 	return delim;

	// }	



	for(int j=0;j<strlen(inputLine);j++)

	{

		if(inputLine[j] == '&' && inputLine[j+1] == '&')

		{

			delim = '&';

			break;

		}

		else if(inputLine[j] == '#' && inputLine[j+1] == '#')

		{

			delim = '#';

			break;

		}

		else if(inputLine[j] == '>')

		{

			delim = '>';

			break;

		}

	}

	if(delim == '0')

	{

		delim = ' ';

	}

	

	if(delim == '&')

	{

		while((currentArg = strsep(&inputLineCopy,"&&")) != NULL)

		{

			char* currentArgCopy = currentArg;

			while((currentArgCopy = strsep(&currentArg," ")) != NULL)

			{

				if(strcmp(currentArgCopy,"") == 0)

				{

					currentArgCopy = NULL;

				}

				myArgs[i] = currentArgCopy; 	

				i++;

			}			

		}

	}

	else if(delim == '#')

	{ 

		while((currentArg = strsep(&inputLineCopy,"##")) != NULL)

		{

			char* currentArgCopy = currentArg;

			while((currentArgCopy = strsep(&currentArg," ")) != NULL)

			{

				if(strcmp(currentArgCopy,"") == 0)

				{

					currentArgCopy = NULL;

				}

				myArgs[i] = currentArgCopy; 	

				i++;

			}			

		}

	}

	else if(delim == '>')

	{

		while((currentArg = strsep(&inputLineCopy,">")) != NULL)

		{

			char* currentArgCopy = currentArg;

			while((currentArgCopy = strsep(&currentArg," ")) != NULL)

			{

				if(strcmp(currentArgCopy,"") == 0)

				{

					currentArgCopy = NULL;

				}

				myArgs[i] = currentArgCopy; 	

				i++;

			}			

		}

	}

	else 

	{

		while((currentArg = strsep(&inputLineCopy," ")) != NULL)

		{

			myArgs[i] = currentArg;

			i++;

		}

	}

	myArgs[i] = NULL;

	// printf("1: %s 2: %s 3: %s 4: %s 5: %s 6: %s 7: %s 8: %s 9: %s",myArgs[0],myArgs[1],myArgs[2],myArgs[3],myArgs[4],myArgs[5],myArgs[6],myArgs[7],myArgs[8]); 

	return delim;

}



void executeCommand(char* myArgs[],size_t size)

{

	if(strcmp(myArgs[0],"cd") == 0)

	{

		chdir(myArgs[1]);

	}	

	else

	{

		// This function will fork a new process to execute a command

		int rc = fork();

		if(rc<0)

		{

			//fork failed

			exit(0);

		}

		else if(rc == 0)

		{

			// Child Process

			signal(SIGINT,SIG_DFL);

			signal(SIGTSTP,SIG_DFL);

			if(myArgs[1] == NULL)

			{

				char* argsToExec[] = {myArgs[0],myArgs[1]};	

				execvp(argsToExec[0],argsToExec);

			}

			else

			{

				char* argsToExec[] = {myArgs[0],myArgs[1],NULL};

				execvp(argsToExec[0],argsToExec);

			}						

			printf("Shell: Incorrect command\n");			

			exit(0); // Makes sure the child process terminates properly

		}

		else

		{

			//parent process

			int rc_wait = wait(NULL); // Makes Deterministic that Child will run before parent 

		}

	}

}



void executeParallelCommands(char* myArgs[],size_t size)

{

	// This function will run multiple commands in parallel

	size_t i=0;

	pid_t rc1 = 0;

	int pid ;

	int check;

	if(strcmp(myArgs[i],"cd") == 0)

	{

		signal(SIGINT,SIG_DFL);

		signal(SIGTSTP,SIG_DFL);

		chdir(myArgs[i+1]);

		i+=2;

	}

	while(1)

	{

		rc1 = fork();

		if(rc1<0)

		{

			exit(0);

		}

		else if(rc1==0)

		{

			//child process

			signal(SIGINT,SIG_DFL);

			signal(SIGTSTP,SIG_DFL);



			if(strcmp(myArgs[i],"cd") == 0)

			{

				chdir(myArgs[i+1]);

			}

			else

			{

				// printf("%s\n",myArgs[i]);

				if(myArgs[i+1] == NULL)

				{

					char* argsToExec[] = {myArgs[i],myArgs[i+1]};	

					execvp(argsToExec[0],argsToExec);

				}

				else

				{

					char* argsToExec[] = {myArgs[i],myArgs[i+1],NULL};

					execvp(argsToExec[0],argsToExec);

				}			

				printf("Shell: Incorrect command\n");			

				exit(0);

			}

		}

		i+=2;

		while((i<size) && (myArgs[i] == NULL))

		{

			i++;

		}	

		if(i>=size)

		{

			break;

		}

	}

	while ((rc1=waitpid(-1,&check,0))!=-1)

	{

		//waiting untill all child process gets terminated

	}

}



void executeSequentialCommands(char* myArgs[],size_t size)

{	

	// This function will run multiple commands in sequential manner

	size_t i=0;

	if(strcmp(myArgs[i],"cd") == 0)

	{

		signal(SIGINT,SIG_DFL);

		signal(SIGTSTP,SIG_DFL);

		chdir(myArgs[i+1]);

		i+=2;

	}

	while(1)

	{

		int rc1 = fork();

		if(rc1<0)

		{

			exit(0);

		}

		else if(rc1==0)

		{

			//child process

			signal(SIGINT,SIG_DFL);

			signal(SIGTSTP,SIG_DFL);



			if(strcmp(myArgs[i],"cd") == 0)

			{

				chdir(myArgs[i+1]);

			}

			else

			{

				if(myArgs[i+1] == NULL)

				{

					char* argsToExec[] = {myArgs[i],myArgs[i+1]};	

					execvp(argsToExec[0],argsToExec);

				}

				else

				{

					char* argsToExec[] = {myArgs[i],myArgs[i+1],NULL};

					execvp(argsToExec[0],argsToExec);

				}

				printf("Shell: Incorrect command\n");			

				exit(0);

			}

		}

		else

		{

			int rc2_wait = wait(NULL);

		}

		i+=2;

		while((i<size) && myArgs[i] == NULL)

		{

			i++;

		}	

		if(i==size)

		{

			break;

		}

	}

}



void executeCommandRedirection(char* myArgs[],size_t size)

{

	// This function will run a single command with output redirected to an output file specificed by user

	

	int rc1 = fork();

	

	if (rc1 < 0)

	{			

		//fork failed

		exit(0);

	}

	else if (rc1 == 0)

	{

		//child processs

		signal(SIGINT,SIG_DFL);

		signal(SIGTSTP,SIG_DFL);



		close(STDOUT_FILENO);

		size_t i=2;

		while(myArgs[i] == NULL)

		{

			i++;

		}	

		open(myArgs[i], O_CREAT | O_WRONLY | O_APPEND);

		

		if(myArgs[1] == NULL)

		{

			char* argsToExec[] = {myArgs[0],myArgs[1]};	

			execvp(argsToExec[0],argsToExec);

		}

		else

		{

			char* argsToExec[] = {myArgs[0],myArgs[1],NULL};

			execvp(argsToExec[0],argsToExec);

		}			

		printf("Shell: Incorrect command\n");			

		exit(0);

	} 

	else 

	{

		int rc_wait = wait(NULL);

	}

}



int main()

{

	// Initial declarations



	while(1)	// This loop will keep your shell running until user exits.

	{

		// Print the prompt in format - currentWorkingDirectory$

		signal(SIGINT, SIG_IGN);

		signal(SIGTSTP, SIG_IGN); // IGN Stands for => ignore



		char* currentWorkingDirectory = getcwd(NULL,0);

        printf("%s$",currentWorkingDirectory);



		// accept input with 'getline()'

		char* inputLine = NULL;

		size_t lenOfInput = 0;

		getline(&inputLine, &lenOfInput, stdin);

		inputLine[strlen(inputLine)-1] = '\0';



		// Finding the number of words(group of characters separated by space) in the input line using strsep() function

		char* inputLineCopy = strdup(inputLine);

		size_t count = 0;

		while(strsep(&inputLineCopy," ") != NULL)

		{

			count++;

		}

		size_t size = 4*count; // we dont require more space according to the way I parsed.

		char* myArgs[size];

		int i;

		for ( i = 0; i < size; i++)

		{

			myArgs[i] = NULL;

		}

		// myArgs[size-1] = NULL;



		// Parse input with 'strsep()' for different symbols (&&, ##, >) and for spaces.

		char delim = parseInput(inputLine,myArgs,size); 

		if (myArgs[0] == NULL)

		{

			continue;

		}

		

		

		if(strcmp(myArgs[0],"exit") == 0)	// When user uses exit command.

		{

			printf("Exiting shell...\n");

			for ( i = 0; i < size; i++)

			{

				myArgs[i] = NULL;

			}

			free(inputLine);

			break;

		}

		

		if(delim == '&')

			executeParallelCommands(myArgs,size);		// This function is invoked when user wants to run multiple commands in parallel (commands separated by &&)

		else if(delim == '#')

			executeSequentialCommands(myArgs,size);	// This function is invoked when user wants to run multiple commands sequentially (commands separated by ##)

		else if(delim == '>')

			executeCommandRedirection(myArgs,size);	// This function is invoked when user wants redirect output of a single command to and output file specificed by user

		else

			executeCommand(myArgs,size);		// This function is invoked when user wants to run a single command

						

		free(inputLine);		

	}	

	return 0;

}


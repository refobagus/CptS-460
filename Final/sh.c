#include "ucode.c"

//skip space
char * spaceDel(char * s)
{
	while(*s == ' ')
		s++;
	return s;
}

void redirection(char * command)
{
	int len = strlen(command);
	int i=0;
	while(i<len)
	{
		//check if outfile or append
		if(command[i] == '>')
		{
			//if append true
			if(i < len - 1 && command[i + 1] == '>')
			{
				command[i] = command[i + 1] = '\0';
				char * filename = command + i + 2;
				filename = spaceDel(filename);
				close(1);
				open(strtok(filename, ' '), O_WRONLY|O_APPEND);
			}
			//outfile
			else
			{
				command[i] = '\0';
				char * filename = command + i + 1;
				filename = spaceDel(filename);
				close(1);
				open(strtok(filename, ' '), O_WRONLY|O_CREAT);
			}
		}
		//do infile
		else if(command[i] == '<')
		{
			command[i] = '\0';
			char * filename = command + i + 1;
			filename = spaceDel(filename);
			close(0);
			open(strtok(filename, ' '), O_RDONLY);
		}
		i++;
	}

}

void commandHelper(char ** commands)
{
	char * head = *commands;
	redirection(head);
	char ** tail = commands + 1;
	if(*tail == NULL)
	{
		exec(head);
		exit(0);
	}
	else
	{
		int pid, dud[2];
		pipe(dud);
		pid = fork();
		if (!pid)
		{
			close(dud[0]);
			dup2(dud[0], 1);
			exec(head);
		
			
		}
		else
		{
			close(dud[1]);
			dup2(dud[1], 0);
			commandHelper(tail);
		}
	}
}

void doCommands(char ** commands)
{
	char copy[100];
	strcpy(copy, *commands);
	char * tok = strtok(copy, ' ');
	// do cd
	if(strcmp(tok, "cd") == 0)
	{
		tok = strtok(NULL, ' ');
		chdir(tok);
		return;
	}

	int pid = fork();
	if(pid)
	{
		int status;
		while(1)
		{
			//if child
			int child = wait(&status);
			if(child == pid) return;
		}
	}
	else
	{
		commandHelper(commands);
	}
}


int main(int argc, char argv[])
{
	char input[1000];
	char *command[50];

	while(1)
	{
		printf("Enter command: ");
		gets(input);

		//if no input run it again
		if(strlen(input) == 0){
			continue;
		}

		// grab first command by searching if the have a pipe. if multiple pipes, go through the loop to grab all commands.
		command[0] = strtok(input, '|');
		for(int i = 1; command[i] = strtok(NULL, '|'); i++);

		//create copy to be safe
		char copy[100];
		strcpy(copy, command[0]);
		//if first command is logout, then quit.
		if(strcmp(copy, "logout") == 0){
			exit(0);
		}

		//loop to check if there is a space and deletes
		for(int i = 1; command[i] != NULL; i++)
			command[i] = spaceDel(command[i]);

		//run grabbed commands
		doCommands(command);
	}

}

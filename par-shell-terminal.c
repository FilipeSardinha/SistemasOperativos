#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "par-shell-terminal.h"

#define TEMP_PIPE_PATH "par-shell-in-stats"
#define MAX_BUFFER_SIZE 100

int parShellTerminal(char* path)
{
	int fd, test;
	char commands[MAX_BUFFER_SIZE];
	while(1)
	{
		fgets(commands, sizeof(commands), 0); 
		
		if(!strncmp(commands, "stats", 5))
		{
			mkfifo(TEMP_PIPE_PATH, O_RDWR);
			fd = open(TEMP_PIPE_PATH, O_RDONLY);
			if(fd < 0)
			{
				printf("Failed to open pipe\n");
				exit(EXIT_FAILURE);
			}
			if(!(close(0)))
			{
				printf("Failed to close pipe\n");
				exit(EXIT_FAILURE);
			}
			if(dup(fd))
			{
				printf("Failed to dup\n");
			}
		}
		if(!strncmp(commands, "exit", 4))
		{
			exit(0);
		}
		else
		{
			fd = open(path, O_WRONLY);
			test = write(fd, commands, sizeof(commands));
			if(test < 0)
			{
				printf("Failed to write on pipe\n");
				exit(EXIT_FAILURE);
			}
		}
	}
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "par-shell-terminal.h"
#include "commandlinereader.h"
#include "list.h"

#define MAXARGS 7
#define MAXPAR 4
#define BUFFER_SIZE 100
#define PIPE_PATH "/tmp/par-shell-in"

pthread_t monitor;
pthread_mutex_t mutex;

FILE* fd = NULL;

pthread_cond_t cond = PTHREAD_COND_INITIALIZER; 
pthread_cond_t slot_cond = PTHREAD_COND_INITIALIZER;
 
int np = 0, exitFlag = 0;
list_t *list;
int iteration = 0;
int total_time = 0;


void *work(void* aux)                  
{
	int pid = 0;
	int status;
	while(1)
	{	
		pthread_mutex_lock(&mutex);
		while(np <= 0 && !exitFlag)
		{
			pthread_cond_wait(&cond, &mutex);
		}										
		pthread_mutex_unlock(&mutex);

		if(np > 0)
		{
			pid = wait(&status);				
			if(pid > 0) 
			{
				pthread_mutex_lock(&mutex);																	
				total_time = update_terminated_process(list, pid, time(NULL), iteration, total_time, fd);
				np--;
				iteration = iteration + 1;
				pthread_cond_signal(&slot_cond);
				pthread_mutex_unlock(&mutex);																							    
			}					 																	
		}
		pthread_mutex_lock(&mutex);																									
		if (exitFlag == 1 && np == 0)
		{
			pthread_mutex_unlock(&mutex); 																									
			return NULL;
		}
		pthread_mutex_unlock(&mutex);																									

	}
	return NULL;
}

int main(int argc, char** argv)               
{
	int test = 0,  p = 0, f = 0;
	char *cmd[MAXARGS];
    char buffer[BUFFER_SIZE];
	char temp[BUFFER_SIZE];

	list = lst_new();

	//unlink the 

	if(unlink(PIPE_PATH) < 0)
	{
		printf("no file to unlink\n");
		//exit(EXIT_FAILURE);
	}

	if(mkfifo(PIPE_PATH, S_IRUSR | S_IWUSR) < 0);	
	{
		printf("Failed to create pipe\nError: %s \n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if(pthread_mutex_init(&mutex, NULL))
	{
		perror("Failed to create the mutex\n");
		exit(EXIT_FAILURE);
	}

	if(pthread_create(&monitor, NULL, work, NULL))
		{
			perror("Failed to create a Monitor\n");
			exit(EXIT_FAILURE);
		}
	if((fd = fopen("log.txt", "a+")))					
	{	
    	while(fgets(temp, sizeof(temp), fd))
		{	
			sscanf(temp,"Total execution time: %d s", &total_time);
			sscanf(temp,"Iteration %d", &iteration);
		}
		iteration = iteration + 1;
	}

	f = open(PIPE_PATH, O_RDONLY);

	close(0);
	dup(f);

	if (parShellTerminal(PIPE_PATH) != 0)
	{
		printf("failed at initialize parshell terminal\n");
	}
		
	while(1)
	{	
		

		p = readLineArguments(cmd, MAXARGS, buffer, BUFFER_SIZE);
		while(p == 0)
		{
			p = readLineArguments(cmd, MAXARGS, buffer, BUFFER_SIZE);
		}
		if(p > 0)
		{ 
			if(strcmp(cmd[0], "exit"))
			{	
				pthread_mutex_lock(&mutex);  
				while(np >= MAXPAR)
				{
					pthread_cond_wait(&slot_cond, &mutex);
				}									
				pthread_mutex_unlock(&mutex);										
				int pid = fork();
				time_t x = time(NULL);
				if(pid == 0)
				{
					test = execv(cmd[0], cmd);				
					if(test < 0)  
					{				
						fprintf(stderr, "Failed, Unkown path\n");
						exit(EXIT_FAILURE);
					}
					else
					{
						exit(EXIT_SUCCESS);
					}
				}
		
				else if(pid > 0)
				{
					pthread_mutex_lock(&mutex); 																		      
					insert_new_process(list, pid, x);
					np++;
					pthread_cond_signal(&cond);
					pthread_mutex_unlock(&mutex); 																			
					p = 0;
				}
				else if(pid < 0)
					perror("Failed creating fork\n");
			}
		}
		if(!(strcmp(cmd[0], "exit")))
		{
			pthread_mutex_lock(&mutex); 
			exitFlag = 1;
			p = 0;
			pthread_cond_signal(&cond);
			pthread_mutex_unlock(&mutex); 
			if(pthread_join(monitor, NULL))
			{
				perror("join failed\n");
				exit(EXIT_FAILURE);
			}
			if(pthread_cond_destroy(&cond))
			{
				perror("Failed to destroy the condition of process\n");
				exit(EXIT_FAILURE);
			}
			if(pthread_cond_destroy(&slot_cond))
			{
				perror("Failed to destroy the condition of the free slots\n");
				exit(EXIT_FAILURE);
			}
			if(pthread_mutex_destroy(&mutex))
			{
				perror("Failed to destroy the mutex\n");
				exit(EXIT_FAILURE);
			}
			fclose(fd);
			lst_destroy(list);
			exit(0);
								
		}
		if(p < 0)
		{
			exit(EXIT_FAILURE);	
		}
	}
	return 0;
}

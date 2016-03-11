/*
 * list.c - implementation of the integer list functions 
 */


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "list.h"



list_t* lst_new()
{
   list_t *list;
   list = (list_t*) malloc(sizeof(list_t));
   list->first = NULL;
   return list;
}


void lst_destroy(list_t *list)
{
	struct lst_iitem *item, *nextitem;

	item = list->first;
	while (item != NULL){
		nextitem = item->next;
		free(item);
		item = nextitem;
	}
	free(list);
}


void insert_new_process(list_t *list, int pid, time_t starttime)
{
	lst_iitem_t *item;

	item = (lst_iitem_t *) malloc (sizeof(lst_iitem_t));
	item->pid = pid;
	item->starttime = starttime;
	item->endtime = 0;
	item->iteration = 0;
	item->total_time = 0;
	item->next = list->first;
	list->first = item;
}


int update_terminated_process(list_t *list, int pid, time_t endtime, int iteration, int total_time, FILE* fd)
{
	lst_iitem_t *item;
	item = list->first;
	int runtime = 0, general_time = 0;	

	while (item != NULL)
	{
		if (item->pid == pid)
		{
			
			item->endtime = endtime;
			item->iteration = iteration;
			item->total_time = total_time;

			runtime = (int)difftime(item->endtime, item->starttime);
			general_time = total_time + runtime;
			//escreve para o ficheiro de log.txt
			//atraves do fd(filedescriptor) escreve para o ficheiro alvo
			fprintf(fd, "Iteration %d\n", item->iteration);
			fprintf(fd, "PID: %d execution time %d s\n", item->pid, runtime);
			fprintf(fd, "Total execution time: %d s\n", general_time);  
			//da flush do ficheiro			
			fflush(fd);
			break;
		}
		item = item->next;
	}

   return general_time;
}


void lst_print(list_t *list)
{
	lst_iitem_t *item;

	printf("Process list with start and end time:\n");
	item = list->first;
	while (item != NULL){
		/*printf("%d, %d\n", (int)item->starttime, (int)item->endtime);*/
		printf("%d\t%.2lfs\n", item->pid, difftime(item->endtime, item->starttime));
		/*printf("%s\n", ctime(&(item->starttime)));
		printf("%s\n", ctime(&(item->endtime)));*/
		
		item = item->next;
	}
	printf("-- end of list.\n");
}

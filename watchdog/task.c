/*
gd -I ../dlist -I ../sorted_list -I ../priqueue -I ../uid sched.c sched_test.c ../dlist/dlist.c  ../sorted_list/slist.c ../priqueue/priQ.c ../uid/UID.c 
*/
#include <stdio.h> /*printf */
#include <time.h> /*time */
#include <stdlib.h> /* malloc */

#include "task.h" /*forward declaration */

int Action(void *param);

struct task
{
	action_t action;
	unsigned int freq;
	ilrd_uid_t id;
	size_t time_to_run;
	void *param; /* param of action */
}; /* task_t */


/* create a task */
/**************************************************************************/
task_t *TaskCreate(action_t Action, unsigned int freq, ilrd_uid_t uid, void *param)
{
	task_t *task = NULL;
	
	task = (task_t*)malloc(sizeof(task_t));
	if(NULL == task)
	{
		fprintf(stderr, "allocation failed\n");
		return task;
	}
	
	task->action = Action;
	task->freq = freq;
	task->id = uid;
	task->param = param;
	task->time_to_run = time(NULL) + task->freq; /*current time + freq */
	
 return task;
}



/* destroy a task */
/**************************************************************************/
void TaskDestroy(task_t *task)
{
	free(task);
	task = NULL;
}



/*returns retVal of Action*/
/**************************************************************************/
int TaskRun(task_t *task)
{
	return task->action(task->param);
}



/* set the task new time to run */
/**************************************************************************/
void TaskSetTime2Run(task_t *task)
{
	task->time_to_run = time(0) + task->freq;
}



/* get the time the task should run */
/**************************************************************************/
size_t TaskGetTime2Run(task_t *task)
{
	return task->time_to_run;
}



/* get the uid of task */
/**************************************************************************/
ilrd_uid_t TaskGetUid(task_t *task)
{
	return task->id;
}










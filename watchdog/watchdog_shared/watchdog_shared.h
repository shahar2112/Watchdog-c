#ifndef __WATCHDOG_SHARED_H__ 
#define __WATCHDOG_SHARED_H__

#ifndef NDEBUG
#define DEBUG_PRINT(s) printf(s)
#define DEBUG_PRINT2(s,v) printf(s,v)
#define WD_EXEC_NAME "./Link to watchdog_proc.Debug.out"
#else
#define DEBUG_PRINT(s)
#define DEBUG_PRINT2(s,v)
#define WD_EXEC_NAME "./Link to watchdog_proc.Release.out"
#endif

/* #ifndef NDEBUG
#else
#endif */


#include <stdio.h>          /* printf()                 */
#include <string.h> 
#include <signal.h>         /* sigaction */
#include <stdlib.h>         /* exit(), malloc(), free() */
#include <unistd.h>       /* getpid() */
#include <pthread.h>
#include <sys/types.h>      /* key_t, sem_t, pid_t      */
#include <semaphore.h>      /* sem_open(), sem_destroy(), sem_wait().. */
#include <fcntl.h>          /* O_CREAT, O_EXEC          */

#define PID_LEN (10)

#include "sched_hpq.h"


sem_t *sem_g = NULL;

 typedef struct params
{
    pid_t other_pid;
    char *other_exec_name;
    char **other_argv;
}params_t;
 
 
pid_t CreateWDProc(int argc, char *argv[]);
int CreateThread(void *params);
void *ThreadFunction(void *params);
int SchedCreateAndRun(void *params);
int SendingSigTask(void *params);
int CheckUid(ilrd_uid_t uid);
void SigUsr1Handler(int signum);
void ClientSigHandler(int signum);
pid_t Spawn(void *params, char *other_env_pid, char *curr_env_pid);
int Revive(void *params);
void SigUsr2Handler(int signum);
int StopTask(void *sched);
void Barrier();
int CleanUp(sched_t *scheduler);
void InitHandlers();
void InitParams(char *argv[]);

#endif
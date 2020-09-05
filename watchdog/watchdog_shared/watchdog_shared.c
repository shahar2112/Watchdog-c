#define RED	printf("\033[1;31m")
#define BLUE printf("\033[1;36m")
#define MAGENTA	printf("\033[1;35m")
#define YELLOW printf("\033[1;33m")
#define GREEN printf("\033[1;32m")
#define DEFAULT	printf("\033[0m")

#include "watchdog_shared.h"

#define PID_COLOR(wd,expected) (wd == expected ? printf("\033[1;33m") : printf("\033[1;36m"))

#define SIG_LIMIT (3)
#define AGR_LIST_LEN (200)
#define MAX_SIGNALS_BEFORE_REVIVE (3)


sig_atomic_t sig_counter_g = 0;
sig_atomic_t sig_to_stop_g = 0;

int CreateThread(void *params)
{
    int ret_val = 0;
    pthread_t thread;

    ret_val = pthread_create(&thread, NULL, ThreadFunction, params);
    ret_val = pthread_detach(thread);
    if (0 != ret_val)
    {
        perror("error creating thread: ");
        return 1;
    }

   return 0;
}


void *ThreadFunction(void *params)
{
    /*opening the sem */
    sem_g = sem_open("/sem", O_CREAT , 0666, 0);
    if(sem_g == SEM_FAILED)
    {
        perror("Failed to create semaphore: ");
        exit(1);
    }

    InitHandlers();

    sem_post(sem_g); 
    
    Barrier();

    if(SchedCreateAndRun(params) == 1)
    {
        fprintf(stderr, "failed to create and run Scheduler");
        exit(1);
    }

    return NULL;
}




int SchedCreateAndRun(void *params)
{
    sched_t *scheduler = NULL;
    ilrd_uid_t client_sig1_uid;
    ilrd_uid_t client_sig2_uid;

    scheduler = SchedCreate();
    if(NULL == scheduler)
	{
		perror("Error. Allocation was unsuccessful: ");
		return 1; 
	}

    client_sig1_uid = SchedAdd(scheduler, SendingSigTask, 2, params);
	if(CheckUid(client_sig1_uid))
    {
        return 1;
    }

    client_sig2_uid = SchedAdd(scheduler, StopTask, 1, scheduler);
	if(CheckUid(client_sig2_uid))
    {
        return 1;
    }

    SchedRun(scheduler);
    CleanUp(scheduler);

    return 0;
}






void SigUsr1Handler(int signum)
{
    const char *msg = NULL;

    PID_COLOR(atoi(getenv("WD_PID")), getpid());
    DEBUG_PRINT("I am SigUsr1Handler initializing counter\n");

    sig_counter_g = 0;
}




void SigUsr2Handler(int signum)
{
    const char *msg = NULL;

    PID_COLOR(atoi(getenv("WD_PID")), getpid());
    DEBUG_PRINT("I am killing WD group \n");

    sig_to_stop_g = 1;
}




int Revive(void *params)
{
    pid_t new_pid;

    if(getpid() == atoi(getenv("WD_PID")))
    {
        BLUE;
        DEBUG_PRINT("---->>>Reviving Client\n");
        DEFAULT;

        new_pid = Spawn(params, "CLIENT_PID", "WD_PID");
        if(new_pid < 0)
        {
            return 1;
        }
    }
    else
    {
        YELLOW;
        DEBUG_PRINT("---->>>Reviving WD\n");
        DEFAULT;
    
        new_pid = Spawn(params, "WD_PID", "CLIENT_PID");
        if(new_pid < 0)
        {
            return 1;
        }
    }
    ((params_t*)params)->other_pid = new_pid; 

    GREEN;
    DEBUG_PRINT("Reviving succeeded :)\n");
    DEFAULT;

    return 0;
}



void InitHandlers()
{
    struct sigaction sa_sendsig;
    struct sigaction sa_terminate;

    sa_sendsig.sa_handler = &SigUsr1Handler;
    sigfillset(&sa_sendsig.sa_mask);

    sa_terminate.sa_handler = &SigUsr2Handler;
    sigfillset(&sa_terminate.sa_mask);

    if(sigaction(SIGUSR1, &sa_sendsig, NULL) == -1)
    {
        perror("Failed to set SIGUSR1 handler: ");
        exit(1); 
    }

    if(sigaction(SIGUSR2, &sa_terminate, NULL) == -1)
    {
        perror("Failed to set SIGUSR2 handler: ");
        exit(1); 
    }
}



int SendingSigTask(void *params)
{
    if(sig_counter_g < SIG_LIMIT)
    {
        if(kill(((params_t*)params)->other_pid, SIGUSR1) != 0)
        {
            RED;
            perror("couldn't send signal: ");
            DEFAULT;
        }
    }
    sig_counter_g++;

    PID_COLOR(atoi(getenv("WD_PID")), getpid());
    DEBUG_PRINT2("    COUNTER IS %d\n", sig_counter_g);
    DEBUG_PRINT2("I have just sent a signal to the process %d\n", ((params_t*)params)->other_pid);
    
    if(sig_counter_g == SIG_LIMIT)
    {
        RED;
        DEBUG_PRINT("Someone died!\n");
        DEFAULT;
        kill(((params_t*)params)->other_pid, SIGKILL);
        sem_wait(sem_g);
        if(Revive(params) == 1)
        {
            return 0; /* fail */
        }    
    }

    return 1;
}



int StopTask(void *sched)
{
    if(sig_to_stop_g == 1)
    {
        RED;
        DEBUG_PRINT("stopping scheduler\n");
        DEFAULT;
        SchedStop((sched_t *)sched);
    }
    return 1;
}




int CleanUp(sched_t *scheduler)
{
    if(scheduler != NULL)
    {
	    SchedDestroy(scheduler);
    }

    sem_unlink("/sem");

    unsetenv("WD_PID");
    unsetenv("CLIENT_PID");

    return 0;
}




int CheckUid(ilrd_uid_t UID)
{
	if(UIDIsEqual(UID, GetBadUid()))
	{
        RED;
		DEBUG_PRINT("didn't add task \n" );
        DEFAULT;
        return 1;
	}

    return 0; 
}





pid_t Spawn(void *params, char *other_env_pid, char *curr_env_pid)
{
    pid_t new_pid;
    pid_t current_proc_pid;
    char current_proc_string_pid[PID_LEN];
    char other_proc_string_pid[PID_LEN];

    current_proc_pid = getpid();
    new_pid = fork();

    if(new_pid == 0)
    {
        sprintf(current_proc_string_pid, "%d", current_proc_pid);
        setenv(curr_env_pid, current_proc_string_pid, 1);

        execvp(((params_t*)params)->other_exec_name, ((params_t*)params)->other_argv);
        RED;
        perror("an error has accured at execvp: ");
        DEFAULT;
        abort();
    }
    else
    {
        sprintf(other_proc_string_pid, "%d", new_pid);
        setenv(other_env_pid, other_proc_string_pid, 1);
    }
    
    return new_pid;
}



void Barrier()
{
    int sval = 0;

    while(1)
    {
        sem_getvalue(sem_g, &sval);
        if(sval == 2)
        {
            return;
        }
    }
}



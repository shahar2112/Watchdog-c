#include "watchdog.h"
#include "watchdog_shared.h"
#include "sched_hpq.h"

#define RED	printf("\033[1;31m")
#define BLUE printf("\033[1;36m")
#define DEFAULT	printf("\033[0m")

params_t params;

status_t MakeMeImmortal(int argc, char *argv[])
{
    pid_t wd_process_pid;

    InitParams(argv);

    if(!getenv("WD_PID"))
    {
        sem_g = sem_open("/sem", O_CREAT , 0666, 0);
        if(sem_g == SEM_FAILED)
        {
            perror("Failed to create semaphore: ");
            return STAT_FAIL;
        }
        sem_init(sem_g, 1, 0);

        wd_process_pid = Spawn(&params, "WD_PID", "CLIENT_PID");
        if( wd_process_pid < 0)
        {
            perror("failed to start watchdog: ");
            return STAT_FAIL;
        }
        params.other_pid = wd_process_pid;

        BLUE;
        DEBUG_PRINT2("--> I AM CLIENT PROCESS with pid %d \n", getpid());
        DEFAULT;
    }
    else
    {
        params.other_pid = atoi(getenv("WD_PID"));
    }

    if(CreateThread(&params) == STAT_FAIL)
    {
        perror("failed to create thread: ");
        return STAT_FAIL;
    }
    
    return STAT_SUCCESS;
}



void DNR()
{
    if(killpg(getpid(), SIGUSR2) != 0)
    {
        RED;
        perror("couldn't send signal: ");
        DEFAULT;
    }
}


void InitParams(char *argv[])
{
    char client_process_string_pid[PID_LEN];

    sprintf(client_process_string_pid, "%d", getpid());
    setenv("CLIENT_PID", client_process_string_pid, 1);
    params.other_exec_name = WD_EXEC_NAME;
    params.other_argv = argv;
}
#include "watchdog_shared.h"

#define RED	printf("\033[1;31m")
#define BLUE printf("\033[1;36m")
#define MAGENTA	printf("\033[1;35m")
#define YELLOW printf("\033[1;33m")
#define GREEN printf("\033[1;32m")
#define DEFAULT	printf("\033[0m")

params_t params;

int main(int argc, char *argv[])
{
    char wd_process_string_pid[PID_LEN];

    YELLOW;
    DEBUG_PRINT2("I AM WATCHDOG PROCESS with pid %d <--\n", getpid());
    DEFAULT;

    sprintf(wd_process_string_pid, "%d", getpid());
    setenv("WD_PID", wd_process_string_pid, 1);

    params.other_argv = argv;
    params.other_exec_name = argv[0];
    params.other_pid = atoi(getenv("CLIENT_PID"));

    if(CreateThread(&params) == 1)
    {
        return 1;
    };

    while(1)
    {}

    return 0;
}


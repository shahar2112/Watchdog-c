#include <time.h>

#include "watchdog.h"

int main(int argc, char *argv[])
{
    int time_to_stop = 10;
    time_t start = time(0);

    if(MakeMeImmortal(argc, argv) == STAT_SUCCESS)
    {};

    while(time(0) - start < time_to_stop)
    {}

    /* DNR(); */ 

    while(1)
    {}

    return 0;
}

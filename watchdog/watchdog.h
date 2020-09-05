#ifndef __WATCHDOG_H__ 
#define __WATCHDOG_H__

#include <stddef.h> /* for size_t */

typedef enum
{
    STAT_SUCCESS,
    STAT_FAIL
}status_t;

status_t MakeMeImmortal(int argc, char *argv[]);

void DNR();

#endif	/* #ifndef watchdog.h  */
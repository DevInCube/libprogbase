/** @file
    @brief Clock module
*/

#pragma once 

//#define _XOPEN_SOURCE 700  // to enable struct timespec and clock_gettime


#define _XOPEN_SOURCE 700
//#define _POSIX_C_SOURCE 200112L
#include <sys/stat.h>
#include <time.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#include <sys/timeb.h>
	#include <sys/types.h>
	#include <winsock2.h>
#endif

/**
    @struct Clock
    @brief encapsulates clock data
*/
typedef struct Clock Clock;
struct Clock {
    struct timespec time;
};

Clock Clock_now(void);
double Clock_diffMillis(Clock c1, Clock c2);

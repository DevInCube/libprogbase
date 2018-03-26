/** @file
    @brief Clock module
*/

#pragma once 

#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif /* __STDC_VERSION__ */
// #define _XOPEN_SOURCE 700  // to enable struct timespec and clock_gettime

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

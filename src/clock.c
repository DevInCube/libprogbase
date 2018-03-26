#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <progbase/clock.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#define CLOCK_REALTIME 0 
	int clock_gettime(int X, struct timespec *tv);
#endif

/* Clock implementations */

Clock Clock_now(void) {
    Clock clock;
    clock_gettime(CLOCK_REALTIME, &(clock.time));
    return clock;
}

static double _getMillis(struct timespec * t1, struct timespec * t2) {
    double ms1 = (double)t1->tv_sec * 1000 + (double)t1->tv_nsec / 1000000.0;
    double ms2 = (double)t2->tv_sec * 1000 + (double)t2->tv_nsec / 1000000.0;
    return ms1 - ms2;
}

double Clock_diffMillis(Clock c1, Clock c2) {
    return _getMillis(&c1.time, &c2.time);
}


/* Clock implementation for Windows */
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

// source: https://stackoverflow.com/a/5404467

LARGE_INTEGER getFILETIMEoffset() {
    SYSTEMTIME s;
    FILETIME f;
    LARGE_INTEGER t;

    s.wYear = 1970;
    s.wMonth = 1;
    s.wDay = 1;
    s.wHour = 0;
    s.wMinute = 0;
    s.wSecond = 0;
    s.wMilliseconds = 0;
    SystemTimeToFileTime(&s, &f);
    t.QuadPart = f.dwHighDateTime;
    t.QuadPart <<= 32;
    t.QuadPart |= f.dwLowDateTime;
    return (t);
}

int clock_gettime(int X, struct timespec *tv) {
    LARGE_INTEGER           t;
    FILETIME            f;
    double                  microseconds;
    static LARGE_INTEGER    offset;
    static double           frequencyToMicroseconds;
    static int              initialized = 0;
    static BOOL             usePerformanceCounter = 0;

    if (!initialized) {
        LARGE_INTEGER performanceFrequency;
        initialized = 1;
        usePerformanceCounter = QueryPerformanceFrequency(&performanceFrequency);
        if (usePerformanceCounter) {
            QueryPerformanceCounter(&offset);
            frequencyToMicroseconds = (double)performanceFrequency.QuadPart / 1000000.;
        } else {
            offset = getFILETIMEoffset();
            frequencyToMicroseconds = 10.;
        }
    }
    if (usePerformanceCounter) QueryPerformanceCounter(&t);
    else {
        GetSystemTimeAsFileTime(&f);
        t.QuadPart = f.dwHighDateTime;
        t.QuadPart <<= 32;
        t.QuadPart |= f.dwLowDateTime;
    }

    t.QuadPart -= offset.QuadPart;
    microseconds = (double)t.QuadPart / frequencyToMicroseconds;
    t.QuadPart = microseconds;
    tv->tv_sec = t.QuadPart / 1000000;
    tv->tv_nsec = (t.QuadPart % 1000000) * 1000;  // to nanoseconds
    return (0);
}

#endif

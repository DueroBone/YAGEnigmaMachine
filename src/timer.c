#define _POSIX_C_SOURCE 200809L

#include "timer.h"

double getElapsedTime(struct timespec *prevTime)
{
    struct timespec currentTime;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &currentTime);

    double elapsedTime = (currentTime.tv_sec - prevTime->tv_sec) +
                         (currentTime.tv_nsec - prevTime->tv_nsec) / 1e9;
    *prevTime = currentTime;
    return elapsedTime;
}

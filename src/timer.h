#pragma once
#ifndef TIMER_H
#define TIMER_H

#include <time.h>

/** Returns seconds since the last call */
double getElapsedTime(struct timespec *prevTime);

#endif // TIMER_H

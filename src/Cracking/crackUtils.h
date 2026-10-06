#pragma once
#ifndef CRACKUTILS_H
#define CRACKUTILS_H

#include "../config.h"
#include "scoring.h"
#include "../Machine/machine.h"

ScoredMachine *testAllRotorPositions(Machine *machine, LETTER *text, size_t length, FitnessFunction *fitnessFunction, int numToSave, double *profilingTimes[4]);

/** Profiling times are in the order: processing, compression, scoring, sorting, creatingRotors */
ScoredMachine *testAllRotorPositionsRings(Machine *machine, LETTER *text, size_t length, FitnessFunction *fitnessFunction, int numToSave, int internalNumToSave, double *profilingTimes[5]);

#endif // CRACKUTILS_H
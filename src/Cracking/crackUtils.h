#pragma once
#ifndef CRACKUTILS_H
#define CRACKUTILS_H

#include "../config.h"
#include "scoring.h"
#include "../Machine/machine.h"

ScoredMachine *testAllRotorPositions(Machine *machine, LETTER *text, size_t length, FitnessFunction *fitnessFunction, int numToSave);

#endif // CRACKUTILS_H
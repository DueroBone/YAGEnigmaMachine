#pragma once
#ifndef SCORING_H
#define SCORING_H

#include <stddef.h>
#include "../config.h"
#include "../Machine/machine.h"

typedef struct
{
    double (*func)(LETTER *text, size_t length);
} FitnessFunction;

typedef struct
{
    CompressedMachine compressedMachine;
    double score;
} ScoredMachine;

double scoreIoc(LETTER *text, size_t length);
double scoreUnigrams(LETTER *text, size_t length);
double scoreBigrams(LETTER *text, size_t length);
double scoreTrigrams(LETTER *text, size_t length);

#endif // SCORING_H
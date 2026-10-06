#include "crackUtils.h"
#include "../Machine/machine.h"
#include "../timer.h"
#include <stdio.h>

ScoredMachine *getTopNMachines(ScoredMachine *scoredMachines, int totalMachines, int numToSave)
{
    ScoredMachine *bestMachines = malloc(numToSave * sizeof(ScoredMachine));
    for (int i = 0; i < numToSave; i++)
    { // Initialize with a low score
        bestMachines[i].score = -1.0;
    }

    for (int i = 0; i < totalMachines; i++)
    {
        for (int j = 0; j < numToSave; j++)
        {
            if (scoredMachines[i].score > bestMachines[j].score)
            { // Bubble down the lower scores to make room for the new high score
                for (int k = numToSave - 1; k > j; k--)
                {
                    bestMachines[k] = bestMachines[k - 1];
                }
                bestMachines[j] = scoredMachines[i];
                break;
            }
        }
    }

    return bestMachines;
}

ScoredMachine *testAllRotorPositions(Machine *machine, LETTER *text, size_t length, FitnessFunction *fitnessFunction, int numToSave, double *profilingTimes[4])
{
    const int totalPositions = 26 * 26 * 26; // Total rotor positions (26^3)
    ScoredMachine scoredMachines[totalPositions];

#ifdef PROFILE_ENABLED
    struct timespec startTime = {0};
    double *proccessingTime = profilingTimes[0];
    double *compressionTime = profilingTimes[1];
    double *scoringTime = profilingTimes[2];
    double *sortingTime = profilingTimes[3];
#endif

    for (int i = 0; i < 26; i++)
    {
        for (int j = 0; j < 26; j++)
        {
            for (int k = 0; k < 26; k++)
            {
#ifdef PROFILE_ENABLED
                int index = (i * 26 * 26) + (j * 26) + k;
                setRotorPositions(machine, i, j, k);
                LETTER output[length];

                getElapsedTime(&startTime); // Reset the timer
                CompressedMachine *compressed = compressMachine(machine);
                scoredMachines[index].compressedMachine = *compressed;
                free(compressed);
                *compressionTime += getElapsedTime(&startTime);

                procLetters(machine, text, output, length);
                *proccessingTime += getElapsedTime(&startTime);

                scoredMachines[index].score = fitnessFunction->func(output, length);
                *scoringTime += getElapsedTime(&startTime);
#else
                int index = (i * 26 * 26) + (j * 26) + k;
                setRotorPositions(machine, i, j, k);
                CompressedMachine *compressed = compressMachine(machine);

                LETTER output[length];
                procLetters(machine, text, output, length);
                double score = fitnessFunction->func(output, length);

                scoredMachines[index].compressedMachine = *compressed;
                free(compressed);
                scoredMachines[index].score = score;
#endif
            }
        }
    }

#ifdef PROFILE_ENABLED
    getElapsedTime(&startTime); // Reset the timer
#endif

    ScoredMachine *bestMachines = getTopNMachines(scoredMachines, totalPositions, numToSave);

#ifdef PROFILE_ENABLED
    *sortingTime += getElapsedTime(&startTime);
#endif

    return bestMachines;
}

ScoredMachine *testAllRotorPositionsRings(Machine *machine, LETTER *text, size_t length, FitnessFunction *fitnessFunction,
                                          int numToSave, int internalNumToSave, double *profilingTimes[5])
{
#ifdef PROFILE_ENABLED
    double *proccessingTime = profilingTimes[0];
    double *compressionTime = profilingTimes[1];
    double *scoringTime = profilingTimes[2];
    double *sortingTime = profilingTimes[3];
    double *creatingRotorsTime = profilingTimes[4];
    struct timespec startTime = {0};
#endif

    ScoredMachine scoredMachines[26 * 26 * 26 * internalNumToSave]; // Total rotor positions (26^3)

    for (int i = 0; i < 26; i++)
    {
        for (int j = 0; j < 26; j++)
        {
            for (int k = 0; k < 26; k++)
            {
#ifdef PROFILE_ENABLED
                getElapsedTime(&startTime); // Reset the timer
#endif
                replaceRotor(machine, 0, machine->rotors[0].rotorNumber, i);
                replaceRotor(machine, 1, machine->rotors[1].rotorNumber, j);
                replaceRotor(machine, 2, machine->rotors[2].rotorNumber, k);
#ifdef PROFILE_ENABLED
                *creatingRotorsTime += getElapsedTime(&startTime);
#endif

                testAllRotorPositions(machine, text, length, fitnessFunction, internalNumToSave, (double *[4]){proccessingTime, compressionTime, scoringTime, sortingTime});
            }
        }
    }
#ifdef PROFILE_ENABLED
    getElapsedTime(&startTime); // Reset the timer
#endif

    ScoredMachine *bestMachines = getTopNMachines(scoredMachines, (26 * 26 * 26 * internalNumToSave), numToSave);

#ifdef PROFILE_ENABLED
    *sortingTime += getElapsedTime(&startTime);
#endif
    return bestMachines;
}
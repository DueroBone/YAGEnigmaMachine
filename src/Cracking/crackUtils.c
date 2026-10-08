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

    const int totalPositions = 26 * 26 * 26 * internalNumToSave;
    ScoredMachine *scoredMachines = malloc((size_t)totalPositions * sizeof(*scoredMachines));
    if (scoredMachines == NULL)
    {
        fprintf(stderr, "Error: Could not allocate rotor position scores\n");
        return NULL;
    }

#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (int i = 0; i < 26; i++)
    {
        for (int j = 0; j < 26; j++)
        {
            for (int k = 0; k < 26; k++)
            {
                Machine threadMachine = *machine;
                double threadProcessingTime = 0;
                double threadCompressionTime = 0;
                double threadScoringTime = 0;
                double threadSortingTime = 0;
                double threadCreatingRotorsTime = 0;
                struct timespec threadStartTime = {0};
                replaceRotor(&threadMachine, 0, threadMachine.rotors[0].rotorNumber, i);
                replaceRotor(&threadMachine, 1, threadMachine.rotors[1].rotorNumber, j);
                replaceRotor(&threadMachine, 2, threadMachine.rotors[2].rotorNumber, k);

#ifdef PROFILE_ENABLED
                getElapsedTime(&threadStartTime); // Reset the timer
                threadCreatingRotorsTime = getElapsedTime(&threadStartTime);
#endif

                ScoredMachine *threadBestMachines = testAllRotorPositions(
                    &threadMachine, text, length, fitnessFunction, internalNumToSave,
#ifdef PROFILE_ENABLED
                    (double *[4]){&threadProcessingTime, &threadCompressionTime, &threadScoringTime, &threadSortingTime}
#else
                    NULL
#endif
                );

                int resultIndex = ((i * 26 + j) * 26 + k) * internalNumToSave;
                for (int result = 0; result < internalNumToSave; result++)
                {
                    scoredMachines[resultIndex + result] = threadBestMachines[result];
                }
                free(threadBestMachines);

#ifdef PROFILE_ENABLED
#ifdef _OPENMP
#pragma omp critical
#endif
                {
                    *proccessingTime += threadProcessingTime;
                    *compressionTime += threadCompressionTime;
                    *scoringTime += threadScoringTime;
                    *sortingTime += threadSortingTime;
                    *creatingRotorsTime += threadCreatingRotorsTime;
                }
#endif
            }
        }
    }

#ifdef PROFILE_ENABLED
    getElapsedTime(&startTime); // Reset the timer
#endif

    ScoredMachine *bestMachines = getTopNMachines(scoredMachines, totalPositions, numToSave);
    free(scoredMachines);

#ifdef PROFILE_ENABLED
    *sortingTime += getElapsedTime(&startTime);
#endif
    return bestMachines;
}
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

ScoredMachine *testAllRotorPositions(Machine *machine, LETTER *text, size_t length, FitnessFunction *fitnessFunction, int numToSave)
{
    const int totalPositions = 26 * 26 * 26; // Total rotor positions (26^3)
    ScoredMachine scoredMachines[totalPositions];

#ifdef PROFILE_ENABLED
    struct timespec startTime = {0};
    double proccessingTime = 0;
    double compressionTime = 0;
    double scoringTime = 0;
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

                CompressedMachine *compressed = compressMachine(machine);
                scoredMachines[index].compressedMachine = *compressed;
                free(compressed);
                compressionTime += getElapsedTime(&startTime);

                getElapsedTime(&startTime); // Reset the timer
                procLetters(machine, text, output, length);
                proccessingTime += getElapsedTime(&startTime);

                scoredMachines[index].score = fitnessFunction->func(output, length);
                scoringTime += getElapsedTime(&startTime);
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
    double sortingTime = getElapsedTime(&startTime);
    printf("Processing time: %f seconds\n", proccessingTime);
    printf("Compression time: %f seconds\n", compressionTime);
    printf("Scoring time: %f seconds\n", scoringTime);
    printf("Sorting time: %f seconds\n", sortingTime);
#endif

    return bestMachines;
}

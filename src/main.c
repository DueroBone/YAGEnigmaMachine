#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Machine/machine.h"
#include "config.h"

int main(void)
{
    double elapsedTime = 0;

    // Create a machine rotors, reflector, ring
    Machine machine;
    initMachine(&machine, (int[3]){1, 2, 3}, 'B', (int[3]){0, 0, 0});

    // Set initial settings
    setRotorPositions(&machine, 0, 0, 0);
    addPlug(&machine, 'A', 'B');
    addPlug(&machine, 'C', 'D');
    addPlug(&machine, 'E', 'F');

    FILE *file = fopen("../data/bible.txt", "r");
    char *input = malloc(4032450);
    fgets(input, 4032450, file);
    fclose(file);
    LETTER *letteredInput = convertStringToLetters(input, strlen(input));

    printf("Input: %s\n", "Hello World");
    printf("Output: %s\n", procString(&machine, "Hello World"));
    // printf("Compression ratio: %zu/%zu bytes\n", sizeof(CompressedMachine), sizeof(Machine));

    struct timespec startTime, endTime;
    LETTER *output = malloc(strlen(input) * sizeof(LETTER));
    for (int i = 0; i < 100; i++)
    {
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &startTime);
        procLetters(&machine, letteredInput, output, strlen(input));
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &endTime);

        elapsedTime += (double)(endTime.tv_sec - startTime.tv_sec) + ((double)(endTime.tv_nsec - startTime.tv_nsec) / 1e9);
    }
    free(output);
    free(letteredInput);
    free(input);

    // Calculate the average elapsed time
    double averageElapsed = elapsedTime / 100.0;
    printf("Average elapsed time: %.4f ms\n", averageElapsed * 1000.0);

    CompressedMachine *compressed = compressMachine(&machine);
    Machine *decompressed = decompressMachine(compressed);

    free(decompressed);
    free(compressed);

    return 0;
}

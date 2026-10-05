#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Machine/machine.h"
#include "config.h"

#define LENGTH 4032450

int main(void)
{
    double elapsedTime;

    // Create a machine with rotors and a reflector
    Machine machine;
    initMachine(&machine, (int[3]){1, 2, 3}, 'B', (int[3]){0, 0, 0});

    // Set initial settings
    setRotorPositions(&machine, 0, 0, 0);
    addPlug(&machine, 'A', 'B');
    addPlug(&machine, 'C', 'D');
    addPlug(&machine, 'E', 'F');

    FILE *file = fopen("bible.txt", "r");
    char *input = malloc(LENGTH);
    fgets(input, LENGTH, file);
    fclose(file);
    LETTER *letteredInput = convertStringToLetters(input, strlen(input));
    free(input);

    // printf("Input: %s\n", "Hello World");
    // printf("Output: %s\n", procString(&machine, "Hello World"));
    // printf("Compression ratio: %zu/%zu bytes\n", sizeof(CompressedMachine), sizeof(Machine));

    struct timespec startTime, endTime;
    LETTER *output = malloc(LENGTH * sizeof(LETTER));
    for (int i = 0; i < 100; i++)
    {

        clock_gettime(CLOCK_MONOTONIC, &startTime);
        procLetters(&machine, letteredInput, output, LENGTH);
        clock_gettime(CLOCK_MONOTONIC, &endTime);

        elapsedTime += ((double)(endTime.tv_nsec - startTime.tv_nsec) / 1e9);
    }
    free(output);
    free(letteredInput);

    // Print the output
    // printf("Input: %s\n", input);
    // printf("Output: %s\n", output);

    // Calculate the average elapsed time
    double averageElapsed = elapsedTime / 100.0;
    printf("Average elapsed time: %.4f ms\n", averageElapsed * 1000.0);

    CompressedMachine *compressed = compressMachine(&machine);
    Machine *decompressed = decompressMachine(compressed);

    free(decompressed);
    free(compressed);

    return 0;
}

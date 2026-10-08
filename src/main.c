#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "config.h"
#include "Machine/machine.h"
#include "private.h"
#include "timer.h"
#include "Cracking/crackUtils.h"
#include "Cracking/scoring.h"

int test()
{

    Machine machine;
    initMachine(&machine, (int[3]){1, 2, 3}, 'B', (int[3]){2, 1, 0});

    // Set initial settings
    addPlug(&machine, 'A', 'B');
    addPlug(&machine, 'C', 'D');
    addPlug(&machine, 'E', 'F');

    char *tested = "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char *expected = "PYQQYQTYLIMHFYMOPSRLHQVPUWQEKCYGUUNNTKJDJWSWROMIOLGSTATRZRRARSJRRAAEEMCYODUEEETUPIQRPXYFZGXACYOJXXKARMTTJKAJLIATMPQOEIBXAVAZZSKAKCRGMUNONQAMEJBQYQRHTIIJFSCFIGDAGMCMTAETKHEUCDEDINBOXFLCQZVNYYCAATKGVFCDOYXZPDSTFNBBONVOTRPQVGSYGVYCZOEQFNTZHHBRVESRLQUAKQGXGGXRPLSAZYZGZHRPAXMXBEICBSCSBUVOUAPYZCVZHPLUMAAUWOLSORVIBUXFPSOVAQDSCAOTQYQJCDHECALULGYGVUABNIKLEIGWEBTKHGLDXEQQJEAIQKITNXTXAJHXBUXZWPJNIXWKNPPJBVDSQRCZBEEYVDDCHZOKJOXHFVUKPDUEORVRJOJOFFDJASCUPFMAUXRFZGIKAIOMFBTROBGUZSOWJHDSWXOXRECTBAWDSTOBGUOSABJBVTWROQRWVXGAWQLQSINJWPFPHLPFBNRKLBHKSTBECQBDXJHNTROBJACNPKYWTVSTYCSEIQHWJEOYBSDSNITHJOICPWGMJJJNITXXXVPMZKFDIEPHLJJVVUJJVZUCRXICFMENHMIJOHQHRHJYYQKIKQPWWHHLOGJNHKVBYSSUBBGWIWHIUWYMTJLLXYHYCZNSWNMFGBKGKOMRTRXBFAEIYHAWYLPZPLVSGWMK";

    setRotorPositions(&machine, 0, 1, 2);
    char *output1 = procString(&machine, tested);
    setRotorPositions(&machine, 0, 1, 2);
    char *output2 = procString(&machine, output1);
    for (int i = 0; i < 52; i++)
    {
        if (output1[i] != expected[i])
        {
            fprintf(stderr, "Error: test[%d] = %c, expected %c\n", i, output1[i], expected[i]);
            free(output1);
            free(output2);
            return 1;
        }
        if (output2[i] != tested[i])
        {
            fprintf(stderr, "Error: test[%d] = %c, expected %c\n", i, output2[i], tested[i]);
            free(output1);
            free(output2);
            return 1;
        }
    }
    free(output1);
    free(output2);
    return 0;
}

char *loadFile(const char *filename, size_t length)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        return NULL;
    }

    char *buffer = malloc(length + 1);
    if (buffer == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for file buffer\n");
        fclose(file);
        return NULL;
    }

    // Read the file content into the buffer skipping all non-letter characters
    size_t actualLength = 0;
    int c;
    while ((c = fgetc(file)) != EOF && actualLength < length)
    {
        if ((c >= 'A' && c <= 'Z'))
        {
            buffer[actualLength++] = (char)c;
        }
        else if ((c >= 'a' && c <= 'z'))
        { // Convert to uppercase
            buffer[actualLength++] = (char)(c - 32);
        }
#ifdef ENABLE_SPACES
        else if (c == ' ')
        {
            buffer[actualLength++] = ' '; // Keep spaces
        }
#endif
    }
    if (actualLength < length)
    {
        fprintf(stderr, "Warning: File %s is shorter than expected length %zu. Actual length: %zu\n", filename, length, actualLength);
    }
    buffer[actualLength] = '\0';

    fclose(file);
    return buffer;
}

int main(void)
{
    if (test())
        return 1;

    printf("Started...\n");

    // Create a machine rotors, reflector, ring
    Machine machine;
    initMachine(&machine, (int[3]){1, 2, 3}, 'B', (int[3]){0, 0, 0});

    // Set initial settings
    setRotorPositions(&machine, 0, 0, 0);
    // addPlug(&machine, 'A', 'B');
    // addPlug(&machine, 'C', 'D');
    // addPlug(&machine, 'E', 'F');

    char *input = loadFile(TEXT_PATH, 5000); // 4032450
    LETTER *letteredInput = convertStringToLetters(input);
    LETTER *encrypted = malloc(strlen(input) * sizeof(LETTER));

#ifdef PROFILE_ENABLED
    printf("Testing avg speed...\n");
    double elapsedTime = 0;
    struct timespec timestamp;
    for (int i = 0; i < 100; i++)
    {

        setRotorPositions(&machine, 3, 2, 1);
        getElapsedTime(&timestamp);
        procLetters(&machine, letteredInput, encrypted, strlen(input));

        elapsedTime += getElapsedTime(&timestamp);
    }

    // Calculate the average elapsed time
    double averageElapsed = elapsedTime / 100.0;
    printf("Average elapsed time: %.4f ms\n", averageElapsed * 1000.0);
#endif

    setRotorPositions(&machine, 3, 2, 1);
    procLetters(&machine, letteredInput, encrypted, strlen(input));

    int numToSave = 10; // Number of top machines to save

    double proccessingTime = 0;
    double compressionTime = 0;
    double scoringTime = 0;
    double sortingTime = 0;
    double creatingRotorsTime = 0;

    ScoredMachine *scoredMachines = testAllRotorPositions(&machine, encrypted, strlen(input), &(FitnessFunction){.func = scoreTrigrams}, numToSave,
                                                          (double *[5]){&proccessingTime, &compressionTime, &scoringTime, &sortingTime});
    printf("\nTop %d machines:\n", numToSave);
    for (int i = 0; i < numToSave; i++)
    {
        Machine *decompressedMachine = decompressMachine(&scoredMachines[i].compressedMachine);
        char *machineString = machineToString(decompressedMachine);

        int numToShow = 44;
        uint8_t exampleDecrypted[numToShow];
        // char *exampleEncryptedString = convertLettersToString(encrypted, numToShow);
        procLetters(decompressedMachine, encrypted, exampleDecrypted, numToShow);
        char *exampleDecryptedString = convertLettersToString(exampleDecrypted, numToShow);

        printf("Score: %.4f, {%s},  Output: %s\n", (scoredMachines[i].score * 100000 + 3000), machineString, exampleDecryptedString);
        free(machineString);
        free(decompressedMachine);
        free(exampleDecryptedString);
        // free(exampleEncryptedString);
    }

#ifdef PROFILE_ENABLED
    printf("\nProfiling times (in seconds):\n");
    printf("Processing time: %.6f\n", proccessingTime);
    printf("Compression time: %.6f\n", compressionTime);
    printf("Scoring time: %.6f\n", scoringTime);
    printf("Sorting time: %.6f\n", sortingTime);
    printf("Creating rotors time: %.6f\n", creatingRotorsTime);
#endif

    free(encrypted);
    free(letteredInput);
    free(input);

    return 0;
}

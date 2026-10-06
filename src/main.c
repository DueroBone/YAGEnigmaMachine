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
    initMachine(&machine, (int[3]){1, 2, 3}, 'B', (int[3]){0, 0, 0});

    // Set initial settings
    addPlug(&machine, 'A', 'B');
    addPlug(&machine, 'C', 'D');
    addPlug(&machine, 'E', 'F');

    char *tested = "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char *expected = "BCJAGNZVJWBRXSNAXORSTNDEMFCHNUNYQSWSQYPBJDKDZFJUCSIUOQVCNTZUHRQRPWJAPDBBZPJZZWDAYYUGYGPITMSRZKGGHLSRALHLDJTJGWYDSWDMUGNZDFCQOMKPFHYUSHLJDMFENDKSIIZSOMAXUBUXZYPSFHOUZGFWEEMFMNIRXPSSDUVDYNWUURLZCFGRYNNKVUKMKWMVFUHBLTRSLTCZGXIIBKWFEHJNLKPAMGGEZPOWJRIONACQHCVTSTQOKDKTTBXFLSCNGRFDLKTSIXGFGCZTLBMIXQNDRNQACRBCTYDNEFXCXSSYDNPAYHTBSJXOGGVNALLWNVIPSFZZJQUYXAWKUCYWXDAFOKOJCRZZAUURLYCFGYGNEKFWIUBTSOYIATQSLQFZGTAKDTLJXOKHDFPCQHOSBYEMQGECHCWGMPOPHZOYTRMRLZSDKJRKYOYIRREANGPHHBCXEXVKFBMFHXWQVWMYOZBZZMTVDGVLSEVCALKHNEKHQIASKUAAYBRXLGFMFPHINNEPHUMUUPIHFSGEVFQTDNWVPDYXRBQGYGNCQIJFEWMVWSRFYOQSUSBYXWTDYOMFZTTOBBLLJMSHAVREAENPRVUBNKPCPYBRHNWVXCSJKPRXQPSVJCYKDLUZNWEYPJQUBACBWKVEDNDZIDOHLKOJHPJFRSOVUPMWJFBWYUJWCYOQSTJRSZSNJNRAXAWERHEWGOASMLBL";

    setRotorPositions(&machine, 0, 0, 0);
    char *output1 = procString(&machine, tested);
    setRotorPositions(&machine, 0, 0, 0);
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

int main(void)
{
    if (test())
        return 1;

    printf("Started...\n");
    double elapsedTime = 0;

    // Create a machine rotors, reflector, ring
    Machine machine;
    initMachine(&machine, (int[3]){1, 2, 3}, 'B', (int[3]){0, 0, 0});

    // Set initial settings
    setRotorPositions(&machine, 0, 0, 0);
    // addPlug(&machine, 'A', 'B');
    // addPlug(&machine, 'C', 'D');
    // addPlug(&machine, 'E', 'F');

    char *input = private_getText();
    LETTER *letteredInput = convertStringToLetters(input);
    LETTER *encrypted = malloc(strlen(input) * sizeof(LETTER));

    // #ifdef PROFILE_ENABLED
    printf("Testing avg speed...\n");
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
    // #endif

    int numToSave = 5; // Number of top machines to save
    ScoredMachine *scoredMachines = testAllRotorPositions(&machine, encrypted, strlen(input), &(FitnessFunction){.func = ioc}, numToSave);
    printf("\nTop %d machines:\n", numToSave);
    for (int i = 0; i < numToSave; i++)
    {
        Machine *decompressedMachine = decompressMachine(&scoredMachines[i].compressedMachine);
        char *machineString = machineToString(decompressedMachine);

        int numToShow = 44;
        uint8_t exampleDecrypted[numToShow];
        char *exampleEncryptedString = convertLettersToString(encrypted, numToShow);
        procLetters(decompressedMachine, encrypted, exampleDecrypted, numToShow);
        char *exampleDecryptedString = convertLettersToString(exampleDecrypted, numToShow);

        printf("Score: %f, Machine: %s, Output: %s\n", scoredMachines[i].score, machineString, exampleDecryptedString);
        free(machineString);
        free(decompressedMachine);
        free(exampleDecryptedString);
        free(exampleEncryptedString);
    }

    free(encrypted);
    free(letteredInput);
    free(input);

    return 0;
}

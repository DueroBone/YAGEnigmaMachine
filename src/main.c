#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "config.h"
#include "Machine/machine.h"
#include "private.h"
#include "timer.h"
#include "Cracking/crackUtils.h"

int test()
{

    Machine machine;
    initMachine(&machine, (int[3]){1, 2, 3}, 'B', (int[3]){0, 0, 0});

    // Set initial settings
    setRotorPositions(&machine, 0, 0, 0);
    addPlug(&machine, 'A', 'B');
    addPlug(&machine, 'C', 'D');
    addPlug(&machine, 'E', 'F');

    char *test = procString(&machine, "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ");
    char *expected = "BCJAGNZVJWBRXSNAXORSTNDEMFCHNUNYQSWSQYPBJDKDZFJUCSIUOQVCNTZUHRQRPWJAPDBBZPJZZWDAYYUGYGPITMSRZKGGHLSRALHLDJTJGWYDSWDMUGNZDFCQOMKPFHYUSHLJDMFENDKSIIZSOMAXUBUXZYPSFHOUZGFWEEMFMNIRXPSSDUVDYNWUURLZCFGRYNNKVUKMKWMVFUHBLTRSLTCZGXIIBKWFEHJNLKPAMGGEZPOWJRIONACQHCVTSTQOKDKTTBXFLSCNGRFDLKTSIXGFGCZTLBMIXQNDRNQACRBCTYDNEFXCXSSYDNPAYHTBSJXOGGVNALLWNVIPSFZZJQUYXAWKUCYWXDAFOKOJCRZZAUURLYCFGYGNEKFWIUBTSOYIATQSLQFZGTAKDTLJXOKHDFPCQHOSBYEMQGECHCWGMPOPHZOYTRMRLZSDKJRKYOYIRREANGPHHBCXEXVKFBMFHXWQVWMYOZBZZMTVDGVLSEVCALKHNEKHQIASKUAAYBRXLGFMFPHINNEPHUMUUPIHFSGEVFQTDNWVPDYXRBQGYGNCQIJFEWMVWSRFYOQSUSBYXWTDYOMFZTTOBBLLJMSHAVREAENPRVUBNKPCPYBRHNWVXCSJKPRXQPSVJCYKDLUZNWEYPJQUBACBWKVEDNDZIDOHLKOJHPJFRSOVUPMWJFBWYUJWCYOQSTJRSZSNJNRAXAWERHEWGOASMLBL";
    for (int i = 0; i < 52; i++)
    {
        if (test[i] != expected[i])
        {
            fprintf(stderr, "Error: test[%d] = %c, expected %c\n", i, test[i], expected[i]);
            free(test);
            return 1;
        }
    }
    free(test);
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
    addPlug(&machine, 'A', 'B');
    addPlug(&machine, 'C', 'D');
    addPlug(&machine, 'E', 'F');

    char *input = private_getText();
    LETTER *letteredInput = convertStringToLetters(input);

    struct timespec timestamp = {0};
    LETTER *output = malloc(strlen(input) * sizeof(LETTER));
    for (int i = 0; i < 100; i++)
    {
        getElapsedTime(&timestamp);
        procLetters(&machine, letteredInput, output, strlen(input));

        elapsedTime += getElapsedTime(&timestamp);
    }

    free(letteredInput);
    free(input);
    free(output);

    // Calculate the average elapsed time
    double averageElapsed = elapsedTime / 100.0;
    printf("Average elapsed time: %.4f ms\n", averageElapsed * 1000.0);

    CompressedMachine *compressed = compressMachine(&machine);
    Machine *decompressed = decompressMachine(compressed);

    free(decompressed);
    free(compressed);

    return 0;
}

#include <string.h>
#include <stdio.h>
#include "../config.h"
#include "machine.h"
#include "rotor.h"
#include "plugboard.h"

void initMachine(Machine *machine, int rotorNumbers[3], int reflectorNumber, int ringSettings[3])
{
    for (int i = 0; i < 3; i++)
    {
        machine->rotors[i] = createRotor(rotorNumbers[i], ringSettings[i]);
    }
    machine->reflector = createRotor(reflectorNumber, 0);

    for (int i = 0; i < 26; i++)
    {
        machine->plugboard.wiring[i] = i;
    }
}

void setRotorPositions(Machine *machine, uint8_t posA, uint8_t posB, uint8_t posC)
{
    setPosition(&machine->rotors[0], posA);
    setPosition(&machine->rotors[1], posB);
    setPosition(&machine->rotors[2], posC);
}

void step(Machine *machine)
{
    Rotor *left = &machine->rotors[0];
    Rotor *middle = &machine->rotors[1];
    Rotor *right = &machine->rotors[2];

    // Determine turnover before moving any rotor. When the middle rotor is at
    // its notch it steps itself and the left rotor, producing the double step.
    bool middleAtNotch = middle->position == middle->turnover;
    bool rightAtNotch = right->position == right->turnover;

    if (middleAtNotch)
        rot_step(left);

    if (rightAtNotch || middleAtNotch)
        rot_step(middle);

    rot_step(right);
}

LETTER procSingle(Machine *machine, LETTER input)
{

    step(machine);

    LETTER letter = input;
#ifdef PROC_AS_CHARS
    char rot1Pos = machine->rotors[0].position + 'A';
    char rot2Pos = machine->rotors[1].position + 'A';
    char rot3Pos = machine->rotors[2].position + 'A';
    LETTER process[9] = {};
    process[0] = pb_proc(&machine->plugboard, letter);
    process[1] = rot_procForward(&machine->rotors[2], process[0]);
    process[2] = rot_procForward(&machine->rotors[1], process[1]);
    process[3] = rot_procForward(&machine->rotors[0], process[2]);
    process[4] = rot_procForward(&machine->reflector, process[3]);
    process[5] = rot_procBackward(&machine->rotors[0], process[4]);
    process[6] = rot_procBackward(&machine->rotors[1], process[5]);
    process[7] = rot_procBackward(&machine->rotors[2], process[6]);
    process[8] = pb_proc(&machine->plugboard, process[7]);
    letter = process[8];
#else
    letter = pb_proc(&machine->plugboard, input);
    letter = rot_procForward(&machine->rotors[2], letter);
    letter = rot_procForward(&machine->rotors[1], letter);
    letter = rot_procForward(&machine->rotors[0], letter);
    letter = rot_procForward(&machine->reflector, letter);
    letter = rot_procBackward(&machine->rotors[0], letter);
    letter = rot_procBackward(&machine->rotors[1], letter);
    letter = rot_procBackward(&machine->rotors[2], letter);
    letter = pb_proc(&machine->plugboard, letter);
#endif

    return letter;
}

void addPlug(Machine *machine, char a, char b)
{
    pb_addPlug(&machine->plugboard, a, b);
}

void removePlug(Machine *machine, char a)
{
    pb_removePlug(&machine->plugboard, a);
}

LETTER *convertStringToLetters(const char *input)
{
    size_t length = strlen(input);
    LETTER *letters = malloc(strlen(input) * sizeof(LETTER));
    if (letters == NULL)
        return NULL;

    for (size_t i = 0; i < length; i++)
    {
        letters[i] = input[i];
        if (letters[i] >= 'a' && letters[i] <= 'z')
        {
            letters[i] -= 32; // Convert to uppercase
        }
#ifdef PROC_AS_CHARS
#else
        if (letters[i] >= 'A' && letters[i] <= 'Z')
        {
            letters[i] -= 'A';
        }
#endif
    }

    return letters;
}

char *convertLettersToString(const LETTER *letters, size_t length)
{
    char *output = malloc(length + 1);
    if (output == NULL)
        return NULL;

    for (size_t i = 0; i < length; i++)
    {
        output[i] = letters[i];
#ifdef PROC_AS_CHARS
#else
        if (output[i] < 26)
        {
            output[i] += 'A';
        }
        else if (output[i] != ' ')
        {
            output[i] = '?'; // Unknown character
        }
#endif
    }
    output[length] = '\0';

    return output;
}

LETTER *procLetters(Machine *machine, const LETTER *input, LETTER *output, size_t length)
{
    for (size_t i = 0; i < length; i++)
    {
#ifdef PROC_AS_CHARS
        if (input[i] >= 'A' && input[i] <= 'Z')
        {
            output[i] = procSingle(machine, input[i]);
        }
        else
        {
            output[i] = input[i];
        }
#else
        {
            if (input[i] < 26)
            {
                output[i] = procSingle(machine, input[i]);
            }
            else
            {
                output[i] = input[i];
            }
        }
#endif
    }

    return output;
}

char *procString(Machine *machine, const char *strInput)
{
    size_t length = strlen(strInput);
    LETTER *letters = convertStringToLetters(strInput);
    if (letters == NULL)
        return NULL;

    LETTER *output = malloc(length + 1);
    if (output == NULL)
    {
        free(letters);
        return NULL;
    }

    procLetters(machine, letters, output, length);

    char *strOutput = convertLettersToString(output, length);
    free(output);
    free(letters);
    return strOutput;
}
#ifdef DUMB_COMPRESSOR
CompressedMachine *compressMachine(Machine *machine)
{
    CompressedMachine *compressed = malloc(sizeof(CompressedMachine));
    if (compressed == NULL)
        return NULL;

    compressed->rotorPositions = 0;
    for (int i = 0; i < 3; i++)
    {
        compressed->rotorPositions *= 26;
        compressed->rotorPositions += machine->rotors[i].position;
    }
    for (int i = 0; i < 3; i++)
    {
        compressed->rotorPositions *= 26;
        compressed->rotorPositions += machine->rotors[i].ring;
    }

    compressed->rotorNumbers = 0;
    for (int i = 0; i < 3; i++)
    {
        compressed->rotorNumbers *= 5;
        compressed->rotorNumbers += machine->rotors[i].rotorNumber;
    }
    compressed->rotorNumbers <<= 1;

    uint8_t plugsCompleted[26] = {0};
    for (int i = 0; i < 26; i++)
    {
        plugsCompleted[i] = machine->plugboard.wiring[i];
    }

    for (int i = 0; i < 26; i++)
    {
        compressed->plugboardWiring[i] = plugsCompleted[i];
    }

    return compressed;
}

Machine *decompressMachine(CompressedMachine *compressed)
{
    Machine *machine = malloc(sizeof(Machine));
    if (machine == NULL)
        return NULL;

    uint8_t rotorNumbers[3] = {0, 0, 0};
    uint8_t reflectorNumber = 0;
    uint8_t ringSettings[3] = {0, 0, 0};
    uint8_t rotorPositions[3] = {0, 0, 0};

    ringSettings[2] = compressed->rotorPositions % 26;
    compressed->rotorPositions /= 26;
    ringSettings[1] = compressed->rotorPositions % 26;
    compressed->rotorPositions /= 26;
    ringSettings[0] = compressed->rotorPositions % 26;
    compressed->rotorPositions /= 26;

    rotorPositions[2] = compressed->rotorPositions % 26;
    compressed->rotorPositions /= 26;
    rotorPositions[1] = compressed->rotorPositions % 26;
    compressed->rotorPositions /= 26;
    rotorPositions[0] = compressed->rotorPositions % 26;

    reflectorNumber = (compressed->rotorNumbers & 1) + 'B';
    compressed->rotorNumbers >>= 1;
    rotorNumbers[2] = compressed->rotorNumbers % 5;
    compressed->rotorNumbers /= 5;
    rotorNumbers[1] = compressed->rotorNumbers % 5;
    compressed->rotorNumbers /= 5;
    rotorNumbers[0] = compressed->rotorNumbers % 5;
    compressed->rotorNumbers /= 5;

    machine->rotors[0] = createRotor(rotorNumbers[0], ringSettings[0]);
    machine->rotors[1] = createRotor(rotorNumbers[1], ringSettings[1]);
    machine->rotors[2] = createRotor(rotorNumbers[2], ringSettings[2]);
    machine->reflector = createRotor(reflectorNumber, 0);

    setRotorPositions(machine, rotorPositions[0], rotorPositions[1], rotorPositions[2]);

    for (int i = 0; i < 20; i++)
    {
        machine->plugboard.wiring[i] = compressed->plugboardWiring[i];
    }

    return machine;
}
#else
CompressedMachine *compressMachine(Machine *machine)
{
    CompressedMachine *compressed = malloc(sizeof(CompressedMachine));
    if (compressed == NULL)
        return NULL;

    for (int i = 0; i < 3; i++)
    {
        compressed->rotorPositions[i] = machine->rotors[i].position;
        compressed->ringSettings[i] = machine->rotors[i].ring;
        compressed->rotorNumbers[i] = machine->rotors[i].rotorNumber;
    }
    compressed->reflectorNumber = machine->reflector.rotorNumber;

    for (int i = 0; i < 26; i++)
    {
        compressed->plugboardWiring[i] = machine->plugboard.wiring[i];
    }

    return compressed;
}

Machine *decompressMachine(CompressedMachine *compressed)
{
    Machine *machine = malloc(sizeof(Machine));
    if (machine == NULL)
        return NULL;

    for (int i = 0; i < 3; i++)
    {
        machine->rotors[i] = createRotor(compressed->rotorNumbers[i], compressed->ringSettings[i]);
        setPosition(&machine->rotors[i], compressed->rotorPositions[i]);
    }
    machine->reflector = createRotor(compressed->reflectorNumber, 0);

    for (int i = 0; i < 26; i++)
    {
        machine->plugboard.wiring[i] = compressed->plugboardWiring[i];
    }

    return machine;
}
#endif

char *machineToString(Machine *machine)
{
    char *output = malloc(100); // Allocate enough space for the output string
    if (output == NULL)
        return NULL;

    snprintf(output, 100, "Rotors: %d %d %d, Reflector: %c, Ring Settings: %2d %2d %2d, Rotor Positions: %2d %2d %2d",
             machine->rotors[0].rotorNumber, machine->rotors[1].rotorNumber, machine->rotors[2].rotorNumber,
             machine->reflector.rotorNumber,
             machine->rotors[0].ring, machine->rotors[1].ring, machine->rotors[2].ring,
             machine->rotors[0].position, machine->rotors[1].position, machine->rotors[2].position);

    return output;
}

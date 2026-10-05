#pragma once
#ifndef MACHINE_H
#define MACHINE_H

#include "rotor.h"
#include "plugboard.h"

typedef struct
{
    Rotor rotors[3];
    Rotor reflector;
    Plugboard plugboard;
} Machine;

typedef struct
{
    uint64_t rotorPositions; // 26*26*26 * 26*26*26    // Rotor positions and ring settings
    uint8_t rotorNumbers;    // 5*5*5 * 2              // Rotor numbers, reflector
    uint8_t plugboardWiring[26];
} CompressedMachine;

/** Ring settings start at 1=a */
void initMachine(Machine *machine, int rotorNumbers[3], int reflectorNumber, int ringSettings[3]);
/** Start at 1=a */
void setRotorPositions(Machine *machine, uint8_t posA, uint8_t posB, uint8_t posC);
void addPlug(Machine *machine, char a, char b);
void removePlug(Machine *machine, char a);

LETTER *convertStringToLetters(const char *input, size_t length);
char *convertLettersToString(const LETTER *letters, size_t length);

char *procString(Machine *machine, const char *strInput);
LETTER *procLetters(Machine *machine, const LETTER *input, LETTER *output, size_t length);

CompressedMachine *compressMachine(Machine *machine);
Machine *decompressMachine(CompressedMachine *compressed);

#endif // MACHINE_H
#pragma once
#ifndef ROTOR_H
#define ROTOR_H

#include <stdint.h>
#include <stdbool.h>
#include "../config.h"

typedef struct
{
    uint8_t position;
    uint8_t turnover;
    uint8_t ring;
    uint8_t rotorNumber;
    // position, letter
    LETTER wiring[26][26];
    LETTER backwardWiring[26][26];
} Rotor;

Rotor createRotor(int number, int ring);

LETTER rot_procForward(Rotor *rotor, LETTER input);
LETTER rot_procBackward(Rotor *rotor, LETTER input);
void rot_step(Rotor *rotor);
void freeRotor(Rotor *rotor);
void setPosition(Rotor *rotor, uint8_t position);

#endif // ROTOR_H

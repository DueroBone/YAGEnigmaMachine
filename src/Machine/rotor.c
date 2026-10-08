#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "rotor.h"
#include "../config.h"

static inline uint8_t c(char c)
{
    return c - 'A';
}

Rotor createRotorAdv(uint8_t position, uint8_t turnover, uint8_t ring,  uint8_t rotorNumber, const char *wiringStr)
{
    Rotor rotor = {0};

    rotor.position = position;
    rotor.turnover = turnover;
    rotor.ring = ring;
    rotor.rotorNumber = rotorNumber;

    // Build initial wirings in both directions
    for (int i = 0; i < 26; i++)
    {
        int wiringIndex = i - ring;
        if (wiringIndex < 0)
            wiringIndex += 26;

        LETTER letter = c(wiringStr[wiringIndex]) + ring;
        if (letter >= 26)
            letter -= 26;

        rotor.wiring[0][i] = letter;
        rotor.backwardWiring[0][letter] = i;
    }

    // Precompute all positions -> outputs
    for (int rotorPosition = 1; rotorPosition < 26; rotorPosition++)
    {
        for (int input = 0; input < 26; input++)
        {
            int shiftedInput = input + rotorPosition;
            if (shiftedInput >= 26)
                shiftedInput -= 26;

            int forward = rotor.wiring[0][shiftedInput] - rotorPosition;
            if (forward < 0)
                forward += 26;
            rotor.wiring[rotorPosition][input] = forward;

            int backward = rotor.backwardWiring[0][shiftedInput] - rotorPosition;
            if (backward < 0)
                backward += 26;
            rotor.backwardWiring[rotorPosition][input] = backward;
        }
    }
    return rotor;
}

Rotor createRotor(int number, int ring)
{
    /*
    https://www.cryptomuseum.com/crypto/enigma/wiring.htm
    Rotor	    ABCDEFGHIJKLMNOPQRSTUVWXYZ	Notch	Turnover	#
        I	    EKMFLGDQVZNTOWYHXUSPAIBRCJ	Y	    Q       	1
        II	    AJDKSIRUXBLHWTMCQGZNPYFVOE	M	    E       	1
        III	    BDFHJLCPRTXVZNYEIWGAKMUSQO	D	    V       	1
        IV	    ESOVPZJAYQUIRHXLNFTGKDCMWB	R	    J       	1
        V	    VZBRGITYUPSDNHLXAWMJQOFECK	H	    Z       	1
        UKW-A	EJMZALYXVBWFCRQUONTSPIKHGD
        UKW-B	YRUHQSLDPXNGOKMIEBFZCWVJAT
        UKW-C	FVPJIAOYEDRZXWGCTKUQSBNMHL
    */
    switch (number)
    {
    // Rotors
    case 1:
        return createRotorAdv(0, c('Q'), ring, 1, "EKMFLGDQVZNTOWYHXUSPAIBRCJ");
    case 2:
        return createRotorAdv(0, c('E'), ring, 2, "AJDKSIRUXBLHWTMCQGZNPYFVOE");
    case 3:
        return createRotorAdv(0, c('V'), ring, 3, "BDFHJLCPRTXVZNYEIWGAKMUSQO");
    case 4:
        return createRotorAdv(0, c('J'), ring, 4, "ESOVPZJAYQUIRHXLNFTGKDCMWB");
    case 5:
        return createRotorAdv(0, c('Z'), ring, 5, "VZBRGITYUPSDNHLXAWMJQOFECK");

    // Reflectors
    case 'A':
        return createRotorAdv(0, 0, 0, 'A', "EJMZALYXVBWFCRQUONTSPIKHGD");
    case 'B':
        return createRotorAdv(0, 0, 0, 'B', "YRUHQSLDPXNGOKMIEBFZCWVJAT");
    case 'C':
        return createRotorAdv(0, 0, 0, 'C', "FVPJIAOYEDRZXWGCTKUQSBNMHL");
    default:
        return (Rotor){0};
    }
}

LETTER rot_procForward(Rotor *rotor, LETTER input)
{
    return rotor->wiring[rotor->position][input];
}

LETTER rot_procBackward(Rotor *rotor, LETTER input)
{
    return rotor->backwardWiring[rotor->position][input];
}

void rot_step(Rotor *rotor)
{
    rotor->position = rotor->position == 25 ? 0 : rotor->position + 1;
}

void freeRotor(Rotor *rotor)
{
    free(rotor);
}

void setPosition(Rotor *rotor, uint8_t position)
{
    rotor->position = position;
}

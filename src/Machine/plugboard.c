#include "../config.h"
#include "plugboard.h"

void pb_addPlug(Plugboard *plugboard, char a, char b)
{
    uint8_t indexA = a - 'A';
    uint8_t indexB = b - 'A';

    // Swap the connections
    plugboard->wiring[indexA] = indexB;
    plugboard->wiring[indexB] = indexA;
}

void pb_removePlug(Plugboard *plugboard, char a)
{
    uint8_t indexA = a - 'A';
    uint8_t indexB = plugboard->wiring[indexA];

    // Reset the connections to their original state
    plugboard->wiring[indexA] = indexA;
    plugboard->wiring[indexB] = indexB;
}

LETTER pb_proc(Plugboard *plugboard, LETTER input)
{
#ifdef PROC_AS_CHARS
    return plugboard->wiring[input - 'A'] + 'A';
#else
    return plugboard->wiring[input];
#endif
}

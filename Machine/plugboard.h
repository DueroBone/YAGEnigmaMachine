#pragma once
#ifndef PLUGBOARD_H
#define PLUGBOARD_H

#include <stdint.h>
#include <stdlib.h>
#include "../config.h"

typedef struct
{
    uint8_t wiring[2 * MAX_PLUGS];
} Plugboard;

void pb_addPlug(Plugboard *plugboard, char a, char b);
void pb_removePlug(Plugboard *plugboard, char a);

LETTER pb_proc(Plugboard *plugboard, LETTER input);

#endif // PLUGBOARD_H

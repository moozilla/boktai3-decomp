#ifndef RESULT_TUTORIAL_H
#define RESULT_TUTORIAL_H

#include "global.h"

struct ResultTutorialLevel {
    u8 pad0[0x50];
    s32 level;
    u8 pad54[0x56];
    s16 timer;
    u8 padAC[3];
    u8 phase;
};

#endif

#ifndef GUARD_ARMOR_ACTION_H
#define GUARD_ARMOR_ACTION_H

#include "global.h"

struct ArmorActionController {
    u8 unk0[0xB3D];
    u8 phase;
    u8 phaseTimer;
    u8 unkB3F;
    u8 actionState;
    u8 unkB41[0xB58 - 0xB41];
    u8 parts[0x1120 - 0xB58];
};

#endif

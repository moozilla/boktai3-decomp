#ifndef GUARD_HEALTH_BURST_H
#define GUARD_HEALTH_BURST_H

#include "global.h"

struct Player;
struct HealthBurstTask {
    u8 unknown0[0x18];
    struct Player *owner;
    u8 unknown1C[0x94 - 0x1c];
    u8 collider[0x54];
    u8 unknownE8[0x34c - 0xe8];
    u16 kind;
    u16 frame;
};

#endif

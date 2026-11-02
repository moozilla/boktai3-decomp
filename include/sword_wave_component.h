#ifndef GUARD_SWORD_WAVE_COMPONENT_H
#define GUARD_SWORD_WAVE_COMPONENT_H

#include "global.h"

struct SwordWaveComponent {
    u8 pad0[6];
    u8 rotation;
    u8 pad7[0x15];
    s16 x, y, z;
    u8 pad22[0x119 - 0x22];
    u8 flags;
    u8 angle;
    u8 pad11B[3];
    u8 active, phase;
    u8 pad120[0x134 - 0x120];
    u16 endX, endY, endZ, pad13A, startX, startY, startZ, pad142;
    s16 dx, dy, dz;
    u16 pad14A;
    u16 fx, fy, fz, pad152, a, b, c, pad15A;
    u8 *parent;
    u8 pad160[4];
    void (*update)(struct SwordWaveComponent *);
    u8 pad168[0x174 - 0x168];
    void (*callback)(struct SwordWaveComponent *);
    u8 padCallback[0xc];
    s32 timer;
    u8 pad188[2];
    s8 multiplier;
    u8 done, pad18C, rotationStep;
};

#endif

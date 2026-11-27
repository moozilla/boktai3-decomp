#ifndef GUARD_INCLUDE_MONSTERS_SWORD_SWORD_RETURN_COMPONENT_LAYOUT_H
#define GUARD_INCLUDE_MONSTERS_SWORD_SWORD_RETURN_COMPONENT_LAYOUT_H

#include "global.h"

struct SwordReturnComponent {
    u8 pad0[0x6];
    u8 rotation;
    u8 pad7[0x15];
    u16 x; /* confidence=low */
    u16 y; /* confidence=low */
    u16 z; /* confidence=low */
    u8 pad22[0x30];
    u16 bodyRadius; /* confidence=low */
    u8 pad54[0x52];
    u16 attackRadius; /* confidence=low */
    u8 padA8[0x58];
    s16 velocityX;
    u16 velocityY; /* confidence=low */
    s16 velocityZ;
    u8 pad106[0x13];
    u8 flags119; /* confidence=low */
    u8 angle;
    u8 pad11B[0x3];
    u8 byte11E; /* confidence=low */
    u8 byte11F; /* confidence=low */
    u8 pad120[0x14];
    u16 endX;
    u16 endY;
    u16 endZ;
    u16 pad13A; /* confidence=low */
    u16 startX; /* confidence=low */
    u16 startY; /* confidence=low */
    u16 startZ; /* confidence=low */
    u16 pad142; /* confidence=low */
    s16 dx; /* confidence=low */
    s16 dy; /* confidence=low */
    s16 dz; /* confidence=low */
    u16 pad14A; /* confidence=low */
    u16 fx;
    u16 fy;
    u16 fz;
    u16 pad152; /* confidence=low */
    u16 a;
    u16 b;
    u16 c;
    u16 pad15A; /* confidence=low */
    u16 *target; /* confidence=low */
    u8 *parent; /* confidence=low */
    u8 pad164[0x10];
    void (*callback)(struct SwordReturnComponent *); /* confidence=low */
    void (*callback178)(struct SwordReturnComponent *); /* confidence=low */
    void (*callback17C)(struct SwordReturnComponent *); /* confidence=low */
    u32 unknown180; /* confidence=low */
    s32 timer; /* confidence=low */
    u8 targetRotation; /* confidence=low */
    u8 pad189; /* confidence=low */
    u8 direction; /* confidence=low */
    u8 done;
    u8 pad18C; /* confidence=low */
    u8 byte1DD; /* confidence=low */
    u8 pad18E[0x2];
};

typedef char SwordReturnComponentSizeCheck[sizeof(struct SwordReturnComponent) == 0x190 ? 1 : -1];

#endif

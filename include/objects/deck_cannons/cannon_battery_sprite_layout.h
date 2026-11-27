#ifndef GUARD_INCLUDE_OBJECTS_DECK_CANNONS_CANNON_BATTERY_SPRITE_LAYOUT_H
#define GUARD_INCLUDE_OBJECTS_DECK_CANNONS_CANNON_BATTERY_SPRITE_LAYOUT_H

#include "global.h"

struct CannonBatterySprite {
    u32 flags;
    u8 pad4[0x3];
    u8 mode;
    u8 pad8[0x8];
    u16 frame;
    u8 pad12[0xa];
};

typedef char CannonBatterySpriteSizeCheck[sizeof(struct CannonBatterySprite) == 0x1c ? 1 : -1];

#endif

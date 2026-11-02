#ifndef GUARD_VANARGAND_DAMAGE_PART_H
#define GUARD_VANARGAND_DAMAGE_PART_H

#include "damage_number_display.h"

struct VanargandDamagePart {
    u8 unknown0[0x1C];
    struct ContactPosition position;
    u8 unknown24[0xA8 - 0x24];
    struct DamageNumberSource damageSource;
    u8 unknownB0[0xFC - 0xB0];
    u16 damage, contactFlags;
    u32 flags;
    u16 animation, breaks;
    u8 unknown108[0x1C];
    u32 pendingFlags;
    u16 pendingDamage, accumulatedDamage, flashTimer;
    u8 unknown12E[2];
    u16 *thresholds;
    s32 *mode;
};

void sub_081A7204(struct VanargandDamagePart *);

typedef char VanargandDamagePartSizeCheck[sizeof(struct VanargandDamagePart) == 0x138 ? 1 : -1];

#endif

#ifndef GUARD_VANARGAND_DAMAGE_PART_H
#define GUARD_VANARGAND_DAMAGE_PART_H

#include "contact_position.h"
#include "damage_number_source.h"

struct VanargandDamagePart {
    u8 unknown0[0x10];
    u16 state; /* 0x10: set 7 when the leap ends (tentative). */
    u8 unknown12[0x1C - 0x12];
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

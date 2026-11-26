#ifndef GUARD_POLLEN_FLOWER_H
#define GUARD_POLLEN_FLOWER_H

#include "scene/collision/contact_swarm.h"

struct CannonPaletteColor {
    u16 value;
} __attribute__((packed, aligned(2)));

struct PollenFlower {
    struct Task task;
    struct ContactPosition initialPosition;
    struct DisplaySequenceSprite sprite;
    struct ContactSwarmGraphics graphics;
    u8 body[0x48];
    struct CannonPaletteColor flashPalette[16];
    void (*state)(void);
    struct DisplaySequenceState sequence;
    s32 rangeSquared;
    s16 effectX, effectY, effectZ, solid;
    s16 timer, flashTimer, variant, spread;
    void *parent;
    u8 padFC[4];
    void *resource;
};

typedef char PollenFlowerSizeCheck[sizeof(struct PollenFlower) == 0x104 ? 1 : -1];

#endif

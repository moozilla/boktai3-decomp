#ifndef GUARD_TASK4E01_PROJECTILE_H
#define GUARD_TASK4E01_PROJECTILE_H

#include "player_cue_sequence.h"

struct Task4E01Bounds {
    s32 x, z;
};

struct Task4E01Collider {
    u8 unknown00[0x1C];
    struct Task4E01Bounds bounds;
};

struct Task4E01Params {
    u16 flags, lifetime;
    u8 unknown04[2];
    s8 direction;
    u8 unknown07;
    s32 speed;
    u8 unknown0C[8];
    u32 damage[5];
    struct Task4E01Bounds bounds;
    u8 unknown30[0x3C - 0x30];
    u32 sprite[3];
    u32 type, element;
    u16 count;
    u16 unknown52;
    struct Task4E01Params *parent;
};

struct Task4E01Projectile {
    u32 frame, flags;
    u8 unknown08[0x18 - 8];
    struct Task4E01Params params;
    u8 unknown70[0x8C - 0x70];
    struct ContactSwarmSprite sprite;
    struct Task4E01Collider collider;
    u8 unknownDC[0x180 - 0xDC];
    struct DisplaySequenceState sequence;
    u32 restarted, animation;
    u8 unknown198[4];
    void *resource;
    s32 radius;
};
void Anim_RestartSequence(struct DisplaySequenceState *, void *, u32, u32, u32);
void Projectile_SetUpdateCallback(void *, void *);
void Projectile_SetCollisionSize(void *, s32, s32, s32);
#endif

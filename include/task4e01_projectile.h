#ifndef GUARD_TASK4E01_PROJECTILE_H
#define GUARD_TASK4E01_PROJECTILE_H
#include "player_cue_sequence.h"
struct Task4E01Params {
    u16 flags, lifetime, tracking;
    s8 direction;
    u32 turnSpeed : 8;
    s32 speed;
    u32 turnParameter;
    s32 (*callback)(void *, s32);
    u32 damage[5];
    struct ContactPosition bounds, offset;
    s16 variant;
    u16 unknown3A;
    u32 sprite[3];
    u32 type, element;
    u16 count, unknown52;
    struct Task4E01Params *parent;
};
struct Task4E01Runtime {
    s32 elapsed, unknown16C, height;
    struct ContactPosition previousPosition;
    s16 turn;
    u16 unknown17E;
    struct DisplaySequenceState sequence;
    u32 restarted, animation;
    u8 unknown198[4];
    void *resource;
    s32 radius;
    s16 trailFrame, cooldown;
    u32 index;
};
struct Task4E01Projectile {
    u32 frame, flags;
    u8 unknown08[8];
    struct ContactPosition velocity;
    struct Task4E01Params params;
    struct ContactSwarmGraphics graphics;
    struct ContactSwarmSprite sprite;
    struct ContactCollider colliders[2];
    struct ContactPosition *target;
    s32 lifetime;
    struct Task4E01Runtime runtime;
};
void Anim_RestartSequence(struct DisplaySequenceState *, void *, u32, u32, u32);
void Projectile_SetUpdateCallback(void *, void *);
void Projectile_SetCollisionSize(void *, s32, s32, s32);
#endif

#ifndef GUARD_PARTICLE_EFFECT_POOL_H
#define GUARD_PARTICLE_EFFECT_POOL_H

#include "global.h"

struct SceneParticleVector {
    u16 x;
    u16 unknown02;
    u16 y;
    u16 unknown06;
};

struct SceneParticle {
    u32 flags;
    u8 unknown04[11];
    u8 mode;
    u8 unknown10[8];
    struct SceneParticleVector position;
    u8 unknown20[8];
    struct SceneParticleVector velocity;
    u16 elapsed;
    u8 state;
    s8 slotIndex;
    u8 reset;
    u8 delay;
    u8 frameTimer;
    u8 framePhase;
    u16 lifetime;
    u8 unknown3A[2];
    u8 baseFrame;
    u8 unknown3D;
    s16 frameInterval;
};

struct SceneParticlePool {
    u8 task[0x18];
    void *resource;
    struct SceneParticle particles[26];
    volatile u32 occupied;
};

typedef void (*SceneParticleUpdate)(struct SceneParticlePool *, struct SceneParticle *);

static inline volatile u32 *SceneParticlePool_SlotMask(struct SceneParticlePool *pool)
{
    return &pool->occupied;
}

static inline u8 SceneParticlePool_IsOccupied(volatile u32 *occupied, s32 index)
{
    if (*occupied & (1 << index))
        return TRUE;
    return FALSE;
}

struct ScriptedColliderActor {
    u8 unknown00[0x1C];
    s16 x;
    s16 y;
    s16 z;
    s16 unknown22;
    u8 unknown24[0x72];
    u8 state;
    u8 unknown97[8];
    u8 enteredState;
};

struct ScriptedColliderPool {
    u8 unknown00[0x3C];
    struct ScriptedColliderActor actors[8];
};

struct ScriptedColliderPosition {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
    u32 unknown : 16;
};

#endif

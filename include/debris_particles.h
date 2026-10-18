#ifndef DEBRIS_PARTICLES_H
#define DEBRIS_PARTICLES_H
#include "breakable_barrier.h"

// Short-lived debris pieces thrown out of a hit barrier. Particles come from the sprite pool
// (SpritePool_Allocate), fall under gravity and blink out after 30 ticks (Debris_Update).

struct DebrisParticle {
    u8 pad0[8];
    u32 timer, flags; // flags bit 0: hidden
    u8 pad10[24];
    struct SceneVec position;
    u8 pad30[60];
    struct SceneVec velocity;
};

struct DebrisParticle *SpritePool_Allocate(void);
void SpritePool_SetResource(void *, u32);
void SpritePool_StartAnimation(void *, u32, u32, u32, u32);
void SpritePool_SetPosition(void *, void *);
u32 SpritePool_Activate(void *, void *, void *);
void SpritePool_AdvanceAnimation(void *);
int Debris_Update(void *unused, struct DebrisParticle *p);

#endif

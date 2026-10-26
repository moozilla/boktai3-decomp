#ifndef BREAKABLE_BARRIER_H
#define BREAKABLE_BARRIER_H
#include "engine.h"
#include "scene_object.h"
#include "render.h"

// Tentative: a task owning up to four wall segments (script 'p' positions, 'd' directions) that
// the player has to destroy. Each segment is a sprite with a collider; hits flash it, spawn debris
// (sub_08012B34) and eventually break it (state 2). state: 0 hidden/waiting, 1 standing, 2 broken.

struct BarrierExtent {
    s32 x : 16, y : 16, z : 16, pad : 16;
};

struct BarrierSprite {
    u32 flags; // bit 0: hidden, bit 2: rotated (direction != 0)
    u8 pad4[3], priority;
    u8 pad8[8];
    u16 frame; // toggled 0/1 as the animation frame counter wraps
    u8 pad12[10];
    struct SceneVec position;
    u8 tail[8];
};

struct BreakableBarrier {
    struct BarrierSprite sprite;
    u8 resource[28];
    struct SceneCollider collider;
    u16 health;
    u8 state, entered, flash, hit, frame, pada3;
    u32 timer;
};

struct BreakableBarrierManager {
    struct Task task;
    u32 count, health, remaining, blend, script;
    u32 direction[4];
    struct SceneVec positions[4];
    struct BreakableBarrier barriers[4];
};

void Collider_SetBodyProperties(void *, s32, s32, s32);
void SparkParticle_Spawn(void *, s32, void *, s32, s32, s32, s32, s32);

/* Hit particles (debris_particles.c) */
s32 sub_08012B34(void *, struct BreakableBarrier *);

#endif

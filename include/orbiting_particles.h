#ifndef GUARD_ORBITING_PARTICLES_H
#define GUARD_ORBITING_PARTICLES_H
#include "engine.h"
#include "scene_vec.h"

/* Eight rising orbs around a terrain-clamped origin; layout from 080187BC–08018D94. */
struct OrbitingParticleSprite {
    u32 flags;                 /* bit 0 hides the effect */
    u8 unknown4[11], priority; /* priority interpretation tentative */
    u8 unknown10[8];
    struct SceneVec position;
    u8 unknown20[8];
};
struct OrbitingParticle {
    u8 state, angle;
    u16 rise, radius, timer; /* timer: respawn delay when idle, animation ticks when active */
    struct OrbitingParticleSprite sprite;
};
struct OrbitingParticles {
    struct Task task;
    s32 time, duration;
    u16 speed, limit, count, frame;
    void *resource;
    struct SceneVec position;
    struct OrbitingParticle orbs[8];
};
u32 OrbitingParticles_HasActiveOrb(struct OrbitingParticles *);
void OrbitingParticles_SpawnOrb(struct OrbitingParticles *, struct OrbitingParticle *);
void OrbitingParticles_Advance(struct OrbitingParticles *, struct OrbitingParticle *);
s32 OrbitingParticles_Update(struct OrbitingParticles *);
u32 OrbitingParticles_Destroy(struct OrbitingParticles *);
s32 OrbitingParticles_Initialize(struct OrbitingParticles *, struct SceneVec *, s32, s32, s32, s32);
struct OrbitingParticles *OrbitingParticles_Create(struct SceneVec *, s32, s32, s32, s32);
struct OrbitingParticles *OrbitingParticles_CreateFromScript(void);
void OrbitingParticles_Stop(struct OrbitingParticles *);

u8 *TerrainModifiers_FindCellOverride(s32, s32);
void sub_08217DE0(void *, void *, s32);
void sub_08217EEC(void *, void *, s32);
void Effect_SetOrigin(void *, s32, s32);
void Effect_SetPalette(void *, u32);
#endif

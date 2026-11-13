#ifndef GUARD_TIMED_EFFECT_MANAGER_H
#define GUARD_TIMED_EFFECT_MANAGER_H
#include "engine.h"
#include "task4e01_projectile.h"
#include "scene_particle_pool.h"
struct TimedEffectTrail {
    struct SceneParticleEffect sprite;
    u8 active, unknown29[7];
};
struct TimedEffectManager {
    struct Task task;
    u32 activeMask;
    s32 counts[4], totalCounts[4];
    s32 capacity;
    u32 trailState;
    void *trailResource;
    struct TimedEffectTrail effects[4];
    struct Task4E01Projectile *entries;
};
void TimedEffect_OnContact(struct ContactCollider *, struct ContactCollider *,
                           struct Task4E01Projectile *);
void TimedEffect_InitEntry(struct TimedEffectManager *, struct Task4E01Projectile *, s32);
void TimedEffect_RemoveFromManager(struct TimedEffectManager *, struct Task4E01Projectile *, u32);
s32 TimedEffect_UpdateTerrainTurn(struct TimedEffectManager *, struct Task4E01Projectile *, s32);
s32 TimedEffect_UpdateManager(struct TimedEffectManager *);
s32 TimedEffect_DestroyManager(struct TimedEffectManager *);
s32 TimedEffect_InitManager(struct TimedEffectManager *, s32);
struct TimedEffectManager *Task4E01_GetOrCreate(void);
struct TimedEffectManager *TimedEffect_GetOrCreateManager(s32);
struct Task4E01Projectile *TimedEffect_FindFreeEntry(struct TimedEffectManager *, s32 *);
#endif

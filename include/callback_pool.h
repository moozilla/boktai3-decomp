#ifndef GUARD_CALLBACK_POOL_H
#define GUARD_CALLBACK_POOL_H
#include "engine.h"
#include "scene_animation.h"
#include "contact_swarm_sprite.h"
#include "contact_collider.h"
#include "display_sequence.h"
#include "scaled_shadow_types.h"
#include "player.h"
struct CallbackPoolEntry {
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState sequence;
    struct SceneAnimation graphics;
    struct ContactCollider collider;
    struct ScaledShadow shadow;
    struct ContactPosition savedPosition;
    u8 unknown120[0x10];
    s32 previousDistanceSquared, distanceSquared, triggerDistanceSquared;
    u32 spawnScript;
    void *healthDisplay;
    u16 damageThreshold;
    u8 spawnIndex, spawnTarget;
    u8 unknown148[2];
    s16 health, maxHealth;
    u8 unknown14E[3], displayMode;
    u16 callbackIndex, unknown154, statusFlags;
    s8 effectIndex;
    u8 unknown159[2], hitTimer, unknown15C[2], defeated, shakeTimer;
    u32 script;
    s32 arguments[8];
};
struct CallbackPoolScriptArgs {
    u32 count : 16, unused : 16;
    s32 *values;
};
struct CallbackPool {
    struct Task task;
    void *sequenceResource;
    struct CallbackPoolEntry entries[4];
    u32 activeMask;
};
void CallbackPool_SpawnHitParticles(struct CallbackPoolEntry *);
void CallbackPool_RefreshHealthDisplay(struct CallbackPoolEntry *);
void ProximityActor_SetState(struct CallbackPoolEntry *, s32);
s32 Pool184_TestStatusFlags(void *, u32);
void Pool184_AddStatusFlags(void *, u16);
static inline void CallbackPool_Subtract(struct ContactPosition *out,
                                         const struct ContactPosition *b, const struct SceneVec *a)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}
#endif

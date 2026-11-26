#ifndef GUARD_REGION_ACTOR_H
#define GUARD_REGION_ACTOR_H
#include "engine/engine.h"
#include "scene/scene_animation.h"
#include "scene/actor/scene_actor_components.h"
#include "effects/projectiles/contact_swarm_sprite.h"
#include "scene/collision/contact_collider.h"
#include "engine/video/display_sequence.h"
#include "scene/terrain/terrain_cell_override.h"
struct RegionActor {
    struct ContactSwarmSprite sprite;
    struct SceneAnimation graphics;
    struct SceneActorBody body;
    struct DisplaySequenceState sequence;
    struct ContactPosition origin;
    struct ContactCollider collider;
    s16 health, maxHealth;
    s32 threshold;
    s16 resourceId, kind;
    u32 behaviorFlags;
    u8 entered, mode;
    u16 frames;
    u8 overlayActive, variant, unknown112[2];
    s32 region;
    struct TerrainCellOverride terrainCell;
};
struct RegionSpriteManager {
    struct Task task;
    void *sequenceResource, *alternateResource;
    struct RegionActor *actors[32];
    u32 used;
    s8 cursor;
};
static inline u8 RegionActor_HasBehavior(struct RegionActor *actor, u32 mask)
{
    if (actor->behaviorFlags & mask)
        return 1;
    return 0;
}
struct RegionActorColliderVector {
    s16 x, y, z;
};
void IceBoulder_UpdateDamageAnimation(struct RegionActor *);
void IceBoulder_UnregisterTerrain(struct RegionActor *);
struct RegionActor *IceBoulderPool_Allocate(struct RegionSpriteManager *);
void IceBoulder_InitializeCollider(struct RegionActor *, s16, s16);
void IceBoulder_ApplyContactDamage(struct ContactCollider *, struct ContactCollider *,
                                       struct RegionActor *);
s32 IceBoulderPool_UpdateActors(struct RegionSpriteManager *);
s32 IceBoulderPool_DestroyActors(struct RegionSpriteManager *);
s32 IceBoulderPool_InitManager(struct RegionSpriteManager *);
void TerrainModifiers_UnregisterCell(void *);
void TerrainModifiers_RegisterCell(void *, s32, s32, s32, s32, s32);
void SceneBody_Init(void *, s32, void *, s32, s32, void *);
void SceneBody_SetSprite(void *, void *);
void TerrainOverlay_QueueAddActor(void *);
u32 TerrainOverlay_QueueRemoveActor(void *);
void SceneBody_Unregister(void *);
#endif

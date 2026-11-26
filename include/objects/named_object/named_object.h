#ifndef GUARD_NAMED_OBJECT_H
#define GUARD_NAMED_OBJECT_H
#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "scene/actor/contact_swarm.h"
#include "scene/actor/scene_actor_components.h"
#include "scene/actor/scene_actor_route.h"

/* Tentative: a scripted scene object (kinds 0-3) from the 12-slot pool owned by
 * gNamedActorManager. Slots are 0x19C bytes. */
struct NamedObjectArgs {
    u16 count, pad;
    u32 *values;
};
struct NamedObject;
typedef void (*NamedObjectHandler)(struct NamedObject *);
struct NamedObject {
    struct ContactSwarmSprite sprite;
    u8 anim[0x1C];
    struct DisplaySequenceState sequence;
    struct ContactPosition motion; /* this frame's movement, carried to attached actors */
    u8 unknown60[0x54];
    s16 index;
    s16 state;
    s16 flags;
    u8 stage, unknownBB;
    s16 timer;
    u16 speed, limit;
    s16 verticalImpulse;
    u32 enterScript, script, exitScript;
    u16 row;
    u8 unknownD2[2];
    s32 lowerHeight, upperHeight;
    u32 name, field_E0;
    NamedObjectHandler handlers[3];
    u8 routeId, routeParameter, routePoint, kind;
    struct SceneActorRouteCursor route;
    struct SceneActorBody body;
    u8 registration[0x10];
    u8 cellRegistrations[4][0x10];
    u8 cellRegistered[4];
};
typedef char NamedObjectSizeCheck[(sizeof(struct NamedObject) == 0x19C) ? 1 : -1];
struct NamedObjectManager {
    struct Task task;
    void *sequences;
    struct NamedObject slots[12];
    u32 used, followName, tetherName;
    u8 movementSoundActive;
};
typedef char NamedObjectManagerSizeCheck[(sizeof(struct NamedObjectManager) == 0x137C) ? 1 : -1];
static inline void NamedObject_EnableFlags(u16 mask, struct NamedObject *object)
{
    object->flags = mask | object->flags;
}
static inline void NamedObject_DisableFlags(u16 mask, struct NamedObject *object)
{
    object->flags &= ~mask;
}
struct NamedObject *NamedObject_AllocateSlot(struct NamedObjectManager *);
void NamedActor_PlayMovementSound(struct NamedObject *);
void NamedObject_TryCapturePlayer(struct NamedObject *);
void NamedObject_ClampTetherAtBarrier(struct NamedObject *);
void NamedObject_UpdateTerrainRegistration(struct NamedObject *);
void NamedObject_UpdateEntryTrigger(struct NamedObject *);
void NamedObject_ClampTetherToPlatform(struct NamedObject *);
void NamedObject_ConstrainPlayer(struct NamedObject *);
void NamedObject_ConstrainTether(struct NamedObject *);
u8 sub_08101094(struct NamedObject *);
s32 Route_GetCurrentPointNodeId(void *);
u32 NamedObject_IsWithinRoutePointBounds(struct SceneActorRouteCursor *,
                                         const struct ContactPosition *, u32, u32);
u8 NamedObject_IsPlayerWithinContactBounds(struct NamedObject *);
struct NamedObject *NamedObject_FindNearPosition(void *);
void Sound_PlaySong(u32);
#endif

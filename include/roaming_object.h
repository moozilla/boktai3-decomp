#ifndef GUARD_ROAMING_OBJECT_H
#define GUARD_ROAMING_OBJECT_H
#include "contact_swarm.h"
#include "scene_actor_components.h"
#include "scene_animation.h"
#include "terrain_shadow.h"
#include "engine_globals.h"
#include "ram.h"
/* Both variants share the body, home-cell probe and target-list entry. */
struct RoamingObjectCellProbe {
    u32 unknown0;
    u16 originalCell, currentCell;
    u8 originalHeight, currentHeight, originalSlope, currentSlope;
    u16 originalFlags, currentFlags;
};
struct RoamingObjectListener {
    void *target;
    u32 flags;
    void *callback;
    struct RoamingObjectListener *next;
};
struct RoamingObject {
    struct Task task;
    u16 id, unknown1A;
    struct SceneActorBody body;
    struct ContactSwarmSprite sprite;
    struct SceneAnimation graphics;
    struct RoamingObjectCellProbe home;
    struct ContactCollider collider;
    struct DisplaySequenceState sequence;
    void *sequenceResource;
    struct RoamingObjectListener listener;
    u16 targetAngle, turnTimer, animation, impulse, state, flashCount;
    void (*update)(struct RoamingObject *);
};
struct RoamingStateTable {
    void (*entries[3])(struct RoamingObject *);
};
struct RoamingObjectExtentHalf {
    u32 first : 16, second : 16;
};
struct RoamingObjectExtent {
    struct RoamingObjectExtentHalf xy, z;
};
void DamageNumberDisplay_Show(void *, s32, u32, void *, void *, u32);
void SceneBody_Init(void *, u16, void *, u32, u32, void *);
void SceneBody_Unregister(void *);
void SceneBody_SetMovementProbe(void *, void *, s32, s32);
void SceneBody_SetSprite(void *, void *);
void CellObject_Init(void *, void *);
void Obj_StepPosition(void *);
void SceneLink_RemoveNode(void *);
s32 TargetRegistry_RegisterActor(void *, void *, void *);
s32 TargetRegistry_RegisterSceneActor(void *, void *, void *);
void RoamingObject_SetState(struct RoamingObject *, u16);
s32 RoamingObject_OnDamage(struct SceneActorBody *, u32, s32);
void RoamingObject_UpdateAnimation(struct RoamingObject *);
void RoamingObject_InitializePosition(struct RoamingObject *);
void RoamingObject_InitHomeBounds(struct RoamingObject *);
void RoamingObject_InitGraphics(struct RoamingObject *);
void RoamingObject_InitCollider(struct RoamingObject *);
u32 RoamingObject_Tick(struct RoamingObject *);
s32 RoamingObject_Destroy(struct RoamingObject *);
s32 RoamingObject_Initialize(struct RoamingObject *, u32, u32);
struct RoamingObject *RoamingObject_Create(u32, u32);
void RoamingObjectB_SetState(struct RoamingObject *, u16);
s32 RoamingObjectB_OnDamage(struct ContactCollider *, u32, s32);
void RoamingObjectB_UpdateAnimation(struct RoamingObject *);
void RoamingObjectB_InitializePosition(struct RoamingObject *);
void RoamingObjectB_InitHomeBounds(struct RoamingObject *);
void RoamingObjectB_InitGraphics(struct RoamingObject *);
void RoamingObjectB_InitCollider(struct RoamingObject *);
u32 RoamingObjectB_Tick(struct RoamingObject *);
s32 RoamingObjectB_Destroy(struct RoamingObject *);
s32 RoamingObjectB_Initialize(struct RoamingObject *, u32, u32);
struct RoamingObject *RoamingObjectB_Create(u32, u32);
#endif

#ifndef GUARD_OBJECTS_BREAKABLE_H
#define GUARD_OBJECTS_BREAKABLE_H
#include "engine/video/anim_sprite.h"
#include "objects/scene_object.h"
#include "engine/task.h"

/*
 * Pooled interactables (tentative): breakable-style props kept in a fixed pool owned by a task
 * (struct PooledInteractableManager). Each slot is 0xEC bytes and embeds a route/motion record,
 * a draw record and a collider. A slot has a small state machine (state 0-4) driven by a
 * jump table; kinds 0-2 pick the animations.
 */

/* Route follower state (copied/stepped by Route_Start and friends). */
struct PooledRoute {
    u32 data[2];
    void *current; /* current route point */
};

struct PooledInteractable {
    u8 active;
    u8 motion; /* 0 static, 1 follows a route, 2 follows a target */
    u8 state;  /* index into the state handler table (0-4) */
    u8 flags;
    u8 entered;  /* set to 1 when a new state starts; the state clears it on first run */
    u8 flash;    /* frames of collision flashing left */
    s16 offset;  /* motion 2: height offset of the follow target */
    s16 timeout; /* frames until the idle state ends (negative: never) */
    u16 delay;   /* motion 1: frames until the next route step */
    s32 counter; /* frames spent in the current state */
    u16 id;      /* script id ('n') */
    u16 script;  /* script run when hit ('p') */
    s32 region;  /* region the slot belongs to, or -1 for all */
    u32 kind;    /* 0-2, selects resource and animations ('D') */
    u32 arg;     /* script argument ('a') */
    struct ContactPosition *target;
    u16 speed, range;
    struct PooledRoute route;
    struct AnimSprite drawable;
    struct SceneCollider collider;
    struct AnimSpriteResource *resource;
};

struct PooledInteractableManager {
    struct Task task;
    u32 count;
    struct PooledInteractable *slots;
    struct AnimSpriteResource resources[3];
};

/* 0x02000034 */
extern struct PooledInteractableManager *gPooledInteractableManager;
extern u16 gUnk_086049C8[];
extern u16 gUnk_086049CE[][2];
extern u16 gUnk_086049DA[];
extern void (*gUnk_086049E0[])(struct PooledInteractableManager *, struct PooledInteractable *);

static inline u8 sameRegion(s32 region)
{
    if (gGameState->region == region)
        return 1;
    return 0;
}

struct PooledPackedArgs {
    u32 flags : 16, pad : 16;
    u32 *args;
};

/* Engine helpers. */
void AnimSprite_InitCleared(void *, void *, s32, s32, s32, s32, s32, void *);
void Anim_SelectPaletteBank(void *, u32);
void Anim_LinkToList(void *, u32);
u32 Route_GetMovementTowardCurrentPoint(void *, void *, void *, void *, u32, u32, u32);
void Route_AdvancePoint(void *);
u16 Route_GetCurrentPointNodeId(void *);
u8 Route_Start(struct PooledRoute *, s32, s32, s32);
void Path_GetPointPosition(struct ContactPosition *, void *, u32);
struct ContactPosition *MovingPlatform_GetPositionByName(s32);
s32 Region_FindContaining(struct ContactPosition *);
u8 *TerrainModifiers_FindCellOverride(s32, s32);

/* Slot helpers and state handlers. */
struct PooledInteractable *LightFixture_FindFreeSlot(struct PooledInteractableManager *);
void LightFixture_EnterState(struct PooledInteractable *, u32);
u32 LightFixture_ConsumeEnteredFlag(struct PooledInteractable *);
void LightFixture_OnHit(struct SceneCollider *, u32, struct PooledInteractable *);
s32 LightFixture_InitSlot(struct PooledInteractableManager *, struct PooledInteractable *);
void LightFixture_RunScript(struct PooledInteractable *);
void LightFixture_NullCallback(void);
void LightFixture_UpdateAnimation(struct PooledInteractableManager *, struct PooledInteractable *);
void LightFixture_Idle(struct PooledInteractableManager *, struct PooledInteractable *);
void LightFixture_React(struct PooledInteractableManager *, struct PooledInteractable *);
void LightFixture_Activate(struct PooledInteractableManager *, struct PooledInteractable *);
void LightFixture_UpdateRouteMotion(struct PooledInteractable *);
void LightFixture_UpdateFollowMotion(struct PooledInteractable *);
s32 LightFixture_UpdateAll(struct PooledInteractableManager *);
s32 LightFixture_DestroyManager(struct PooledInteractableManager *);
s32 LightFixture_InitManager(struct PooledInteractableManager *);
void *LightFixture_GetOrCreateManager(void);
s32 LightFixture_ScriptCreate(void);
void LightFixture_ScriptSetTimeout(void);
void LightFixture_ScriptSetState(void);

#endif

#ifndef POOLED_INTERACTABLE_H
#define POOLED_INTERACTABLE_H
#include "scene_object.h"

/*
 * Pooled interactables (tentative): breakable-style props kept in a fixed pool owned by a task
 * (struct PooledInteractableManager). Each slot is 0xEC bytes and embeds a route/motion record,
 * a draw record and a collider. A slot has a small state machine (state 0-4) driven by a
 * jump table; kinds 0-2 pick the animations.
 */

struct PooledResource {
    u32 data[8]; /* copied animation resource; three are held by the manager */
};

/* Route follower state (copied/stepped by Route_Start and friends). */
struct PooledRoute {
    u32 data[2];
    void *current; /* current route point */
};

struct PooledDrawable {
    u32 pad0[2];
    u32 flags; /* bit 0: hidden */
    u8 pad0C[20];
    struct SceneVec position;
    u8 tail[56];
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
    struct SceneVec *target;
    u16 speed, range;
    struct PooledRoute route;
    struct PooledDrawable drawable;
    struct SceneCollider collider;
    struct PooledResource *resource;
};

struct PooledInteractableManager {
    u8 task[24];
    u32 count;
    struct PooledInteractable *slots;
    struct PooledResource resources[3];
};

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
void Widget_InitCleared(void *, void *, s32, s32, s32, s32, s32, void *);
void Anim_SelectPaletteBank(void *, u32);
void Anim_LinkToList(void *, u32);
u32 sub_0821E67C(void *, void *, void *, void *, u32, u32, u32);
void Route_AdvancePoint(void *);
u16 Route_GetCurrentPointNodeId(void *);
u8 Route_Start(struct PooledRoute *, s32, s32, s32);
void Path_GetPointPosition(struct SceneVec *, void *, u32);
struct SceneVec *NamedObject_GetPositionByName(s32);
s32 Region_FindContaining(struct SceneVec *);
u8 *TerrainModifiers_FindCellOverride(s32, s32);

/* Slot helpers and state handlers. */
struct PooledInteractable *Breakable_FindFreeSlot(struct PooledInteractableManager *);
void Breakable_EnterState(struct PooledInteractable *, u32);
u32 Breakable_ConsumeEnteredFlag(struct PooledInteractable *);
void Breakable_OnHit(struct SceneCollider *, u32, struct PooledInteractable *);
s32 Breakable_InitSlot(struct PooledInteractableManager *, struct PooledInteractable *);
void Breakable_RunScript(struct PooledInteractable *);
void sub_08008C18(void);
void Breakable_UpdateAnimation(struct PooledInteractableManager *, struct PooledInteractable *);
void Breakable_Idle(struct PooledInteractableManager *, struct PooledInteractable *);
void Breakable_React(struct PooledInteractableManager *, struct PooledInteractable *);
void Breakable_Activate(struct PooledInteractableManager *, struct PooledInteractable *);
void Breakable_UpdateRouteMotion(struct PooledInteractable *);
void Breakable_UpdateFollowMotion(struct PooledInteractable *);
s32 Breakable_UpdateAll(struct PooledInteractableManager *);
s32 Breakable_DestroyManager(struct PooledInteractableManager *);
s32 Breakable_InitManager(struct PooledInteractableManager *);
void *Breakable_GetOrCreateManager(void);
s32 Breakable_ScriptCreate(void);
void Breakable_ScriptSetTimeout(void);
void Breakable_ScriptSetState(void);

#endif

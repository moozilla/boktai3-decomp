#ifndef GUARD_ROUTE_SPRITE_ACTOR_H
#define GUARD_ROUTE_SPRITE_ACTOR_H
#include "contact_swarm.h"
/* Tentative four-slot route-driven actor pool; visual/story identity is unknown. */
struct RouteSpriteRouteResource {
    u8 pointCount, unknown1;
    u16 pointsOffset;
};
struct RouteSpriteRoutePoint {
    s16 x, z;
    u16 flags;
    u8 unknown6[2];
};
struct RouteSpriteRoute {
    u8 unknown0, unknown1, index, ended;
    const struct RouteSpriteRouteResource *resource;
    const struct RouteSpriteRoutePoint *current;
};
struct RouteSpriteActor {
    struct ContactSwarmSprite sprite;
    u8 graphics[0x1C];
    struct DisplaySequenceState sequence;
    struct RouteSpriteRoute route;
    u8 shadow[0x6C];
    struct ContactPosition elevatedPosition;
    struct ContactPosition waypoints[12], endpoint;
    struct ContactPosition target, savedPosition;
    u16 flags150;
    u8 state;
    s8 index;
    u16 frames;
    u8 entered, positionChanged, animationEnded, heading;
    s8 angle;
    s8 shadowAbsent;
    u8 waypointIndex, kind, option, variant;
    u32 parameter;
    s16 amount;
    u8 prepared, unknown167;
    u16 cyclesRemaining;
    s16 initialTimer, timer, moveTimer;
    u32 script;
    void *context;
};
struct RouteSpritePool {
    u8 task[0x18];
    void *sequenceResource;
    u32 occupiedMask;
    struct RouteSpriteActor slots[4];
};
typedef void (*RouteSpriteStateCallback)(struct RouteSpritePool *, struct RouteSpriteActor *);
extern RouteSpriteStateCallback gUnk_08605BB0[];
/* 0x02000198 */
extern struct RouteSpritePool *gRouteSpritePool;
struct RouteSpriteActor *RouteSpritePool_AllocateSlot(struct RouteSpritePool *);
struct RouteSpriteActor *RouteSpritePool_TryAllocateSlot(struct RouteSpritePool *);
s32 RouteSpritePool_Tick(struct RouteSpritePool *);
s32 RouteSpritePool_Destroy(struct RouteSpritePool *);
s32 RouteSpritePool_Initialize(struct RouteSpritePool *);
struct RouteSpritePool *RouteSpritePool_Create(void);
void RouteSpriteActor_RunEventScript(struct RouteSpriteActor *);
void sub_0806CFFC(struct RouteSpritePool *, struct RouteSpriteActor *);
u8 sub_0806E308(void);
void ScaledShadow_Destroy(void *);
void RouteSpriteActor_Create(struct ContactPosition *, void *, s32, s32, s32, u32, u32, u32, u32,
                             u32, void *, s32);
#endif

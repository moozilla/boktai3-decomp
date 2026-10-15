#ifndef GUARD_ROUTE_SPRITE_ACTOR_H
#define GUARD_ROUTE_SPRITE_ACTOR_H
#include "contact_swarm.h"
/* Tentative four-slot route-driven actor pool; visual/story identity is unknown. */
struct RouteSpriteActor {
    struct ContactSwarmSprite sprite;
    u8 graphics[0x1C];
    struct DisplaySequenceState sequence;
    u8 route[0xC];
    u8 shadow[0x6C];
    struct ContactPosition elevatedPosition;
    u8 unknownD8[0x68];
    struct ContactPosition target, savedPosition;
    u16 flags150;
    u8 state;
    s8 index;
    u16 frames;
    u8 entered, positionChanged, animationEnded, heading, angle;
    s8 shadowAbsent;
    u8 unknown15C, kind, option, variant;
    u32 parameter;
    s16 amount;
    u8 prepared, unknown167;
    s16 height, initialTimer, timer, moveTimer;
    void *owner, *context;
};
struct RouteSpritePool {
    u8 task[0x18];
    void *sequenceResource;
    u32 occupiedMask;
    struct RouteSpriteActor slots[4];
};
extern struct RouteSpritePool *gUnk_02000198;
struct RouteSpriteActor *sub_0806DF60(struct RouteSpritePool *);
struct RouteSpriteActor *sub_0806DFB8(struct RouteSpritePool *);
struct RouteSpritePool *sub_0806E250(void);
void sub_08013B74(void *);
void sub_0806DFC8(struct ContactPosition *, void *, s32, s32, s32, u32, u32, u32, u32, void *,
                  void *, s32);
#endif

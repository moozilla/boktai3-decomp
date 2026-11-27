#ifndef GUARD_INCLUDE_BOSSES_HRESVELGR_OFFSET_ACTOR_LAYOUT_H
#define GUARD_INCLUDE_BOSSES_HRESVELGR_OFFSET_ACTOR_LAYOUT_H

#include "global.h"

struct OffsetActorAction {
    u8 state;
    u8 pad1[0x3];
    u32 frame;
    u32 unk8; /* confidence=low */
    u32 unkC; /* confidence=low */
};

typedef char OffsetActorActionSizeCheck[sizeof(struct OffsetActorAction) == 0x10 ? 1 : -1];

struct OffsetActor {
    u8 pad0[0x94];
    s16 health;
    s16 maxHealth; /* confidence=low */
    u8 pad98[0x2];
    u8 active;
    u8 pad9B[0xf];
    u8 animation;
    u8 padAB[0x3];
    u8 behavior; /* confidence=low */
    u8 finished; /* confidence=low */
    u8 pending; /* confidence=low */
    u8 padB1[0x13];
    u32 behaviorTick; /* confidence=low */
    u8 padC8[0x238];
    struct OffsetActorAction action;
    u8 pad310[0x8];
    u16 contact;
    u16 pad31A; /* confidence=low */
    u16 pad31C; /* confidence=low */
    u16 pad31E; /* confidence=low */
    u16 pad320; /* confidence=low */
    u16 pad322; /* confidence=low */
    u16 response; /* confidence=low */
    u8 pad326[0x6];
    u16 phase;
    u16 index;
    u8 pad330[0x53];
    u8 ready; /* confidence=low */
};

typedef char OffsetActorSizeCheck[sizeof(struct OffsetActor) == 0x384 ? 1 : -1];

#endif

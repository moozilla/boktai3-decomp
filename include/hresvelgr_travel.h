#ifndef GUARD_HRESVELGR_TRAVEL_H
#define GUARD_HRESVELGR_TRAVEL_H

#include "scene_vec.h"
#include "boss_action_state.h"
#include "scene_input_widget.h"

struct HresvelgrTravelActor {
    u8 pad0[0x51];
    u8 facing;
    u8 pad52[2];
    struct SceneVec position, velocity;
    u8 pad64[0x9A - 0x64];
    u8 vulnerable;
    u8 pad9B;
    u32 flags;
    u8 padA0, behaviorFlags;
    u8 phase, animationCompletions;
    u32 phaseFrame;
    u8 facing4, facing8, requestedAnimation, currentAnimation;
    u8 actionMode, actionIndex, actionPhase, finished;
    u8 actionEnter, changed, pulse;
    u8 padB3[0xCC - 0xB3];
    u8 targetHeading;
    u8 padCD[0x104 - 0xCD];
    void *callback;
    u8 pad108[4];
    struct Player *player;
    struct SceneVec *origin;
    s8 savedHeading;
    u8 pad115[0x5F];
    struct SceneInputResource animationData;
    struct SceneInputWidget animation;
    u8 pad1F4[0x2F0 - 0x1F4];
    struct BossActionState action, behavior; /* 0x2F0, 0x300 */
    u8 pad310[8];
    u16 recoveryFrames;
    u8 pad31A[2];
    s16 motionX, motionY, motionZ, motionW;
    u8 pad324[7];
    u8 side;
    u8 pad32C[4];
    u16 chargeCount;
    u8 pad332[4];
    u16 burstCount;
    struct SceneVec shotOffsets[4];
    u8 pad358[0x383 - 0x358];
    u8 behaviorFinished;
    void *savedCallback;
    u8 savedPhase, savedAnimation;
    s16 travel;
    u8 pad38C[0x10];
    struct SceneVec target, delta;
    u8 pad3AC[0x420 - 0x3AC];
    void *hazards; /* 0x420: growth hazard list visited by Hresvelgr_UpdateHazardVisit. */
    u16 normalPalette[16][16]; /* 0x424: copied from bank 0x2F0 at initialization. */
    u8 pad624[0x704 - 0x624];
    u16 flashPalette[7][16]; /* 0x704: initialized to RGB555 gray. */
};

static inline u8 Hresvelgr_EnterVulnerableBehavior(struct HresvelgrTravelActor *actor)
{
    actor->vulnerable = 1;
    if (actor->actionEnter) {
        actor->actionEnter = 0;
        actor->finished = 0;
        return TRUE;
    }
    return FALSE;
}

static inline void Hresvelgr_SetFlags(struct HresvelgrTravelActor *actor, u32 flags)
{
    actor->flags |= flags;
}

static inline void Hresvelgr_EnterPhase(struct HresvelgrTravelActor *actor, u32 phase)
{
    actor->phase = phase;
    actor->phaseFrame = 0;
}

s32 Hresvelgr_RefreshTarget(struct HresvelgrTravelActor *);

#endif

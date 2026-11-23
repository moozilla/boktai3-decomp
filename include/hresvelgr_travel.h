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
    u8 pad64[0x90 - 0x64];
    u32 tick; /* 0x90: frames in the current behavior (read by the travel phases). */
    s16 health; /* 0x94 */
    u8 pad96[0x9A - 0x96];
    u8 vulnerable;
    u8 pad9B;
    u32 flags;
    u8 padA0, behaviorFlags;
    u8 phase, animationCompletions;
    u32 phaseFrame;
    u8 facing4, facing8, requestedAnimation, currentAnimation;
    u8 actionMode, actionIndex, actionPhase, finished;
    u8 actionEnter, changed, pulse;
    u8 padB3[0xC4 - 0xB3];
    u32 actionTimer; /* 0xC4: StateActor actionTimer. */
    u8 padC8[0xCC - 0xC8];
    u8 targetHeading;
    u8 padCD[0x100 - 0xCD];
    void *actionCallback; /* 0x100: StateActor actionCallback. */
    void *callback;
    u8 pad108[4];
    struct Player *player;
    struct SceneVec *origin;
    s8 savedHeading;
    u8 pad115[0x5F];
    struct SceneInputResource animationData;
    struct SceneInputWidget animation;
    u8 pad1F4[0x248 - 0x1F4];
    u8 bodyCollider[0x2F0 - 0x248]; /* 0x248: set up by ColliderActor_InitBodyCollider. */
    struct BossActionState action, behavior; /* 0x2F0, 0x300 */
    u16 reactionStage; /* 0x310: set 6 by ActorReaction_BeginCollisionPhase (tentative). */
    u8 pad312[6];
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
    struct SceneVec bodyOffset; /* 0x38C */
    struct SceneVec bodyRotation; /* 0x394 */
    struct SceneVec target, delta;
    u8 pad3AC[0x420 - 0x3AC];
    void *hazards; /* 0x420: growth hazard list visited by Hresvelgr_UpdateHazardVisit. */
    u16 normalPalette[16][16]; /* 0x424: copied from bank 0x2F0 at initialization. */
    u8 pad624[0x704 - 0x624];
    u16 flashPalette[7][16]; /* 0x704: initialized to RGB555 gray. */
    void *linked; /* 0x7E4: actor notified by Hresvelgr_BeginLinkedAnimation. */
};

/* StateActor_ConsumeActionEntry on this type. */
static inline u8 Hresvelgr_ConsumeActionEntry(struct HresvelgrTravelActor *actor)
{
    if (actor->actionEnter) {
        actor->actionEnter = 0;
        actor->finished = 0;
        return TRUE;
    }
    return FALSE;
}

static inline u8 Hresvelgr_EnterVulnerableBehavior(struct HresvelgrTravelActor *actor)
{
    actor->vulnerable = 1;
    return Hresvelgr_ConsumeActionEntry(actor);
}

static inline u32 Hresvelgr_TestFlags(const struct HresvelgrTravelActor *actor, u32 mask)
{
    return actor->flags & mask;
}

static inline void Hresvelgr_SetFlags(struct HresvelgrTravelActor *actor, u32 flags)
{
    actor->flags |= flags;
}

/* SceneActorState_SetAnimation on this type: requestedAnimation is the StateActor's
 * actionAnimation (0xAA). */
static inline void Hresvelgr_SetActionAnimation(struct HresvelgrTravelActor *actor, u32 animation)
{
    actor->requestedAnimation = animation;
}

/* SceneActorState_StartAction on this type. */
static inline void Hresvelgr_StartAction(struct HresvelgrTravelActor *actor, u32 mode, u32 index,
                                         u32 phase)
{
    actor->actionMode = mode;
    actor->actionIndex = index;
    actor->actionPhase = phase;
    actor->actionTimer = 0;
    actor->actionEnter = 1;
}

/* SceneActorState_StartReaction on this type: hand the actor to a reaction callback
 * as reaction state STATE (tick, currentAnimation, phase and phaseFrame are the
 * StateActor's callbackFrame, actionState, reactionPhase and reactionTimer). */
static inline void Hresvelgr_StartReaction(struct HresvelgrTravelActor *actor,
                                           void (*callback)(void *), int state)
{
    actor->currentAnimation = state;
    actor->actionCallback = callback;
    actor->tick = 0;
    actor->changed = 1;
    actor->phase = 0;
    actor->phaseFrame = 0;
}

static inline void Hresvelgr_EnterPhase(struct HresvelgrTravelActor *actor, u32 phase)
{
    actor->phase = phase;
    actor->phaseFrame = 0;
}

s32 Hresvelgr_RefreshTarget(struct HresvelgrTravelActor *);

#endif

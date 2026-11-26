#ifndef GUARD_STATE_ACTOR_ACTION_ENTRY_H
#define GUARD_STATE_ACTOR_ACTION_ENTRY_H

#include "npc/story_character/state_actor.h"

static inline u8 StateActor_ConsumeActionEntry(struct StateActor *actor)
{
    if (actor->actionEnter) {
        actor->actionEnter = 0;
        actor->actionFinished = 0;
        return TRUE;
    }
    return FALSE;
}

/* The recovery-pipeline operations every action state shares. The shared states in
 * actor_graphics_and_basic_states.c (0x08021EA8-0x08024F2C) use them at some thirty
 * sites under these names; the bosses' own states repeat them. SceneActorState_TestFlags and
 * SceneActorState_EnterAction (the reaction start) are in state_actor.h. */
static inline void SceneActorState_SetAnimation(struct StateActor *actor, u32 animation)
{
    actor->actionAnimation = animation;
}

static inline void SceneActorState_EnterPhase(struct StateActor *actor, u32 phase)
{
    actor->actionPhase = phase;
    actor->actionTimer = 0;
    actor->actionEnter = 1;
}

static inline void SceneActorState_StartAction(struct StateActor *actor, u32 mode, u32 index,
                                               u32 phase)
{
    actor->actionMode = mode;
    actor->actionIndex = index;
    SceneActorState_EnterPhase(actor, phase);
}

#endif

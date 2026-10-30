#ifndef GUARD_SCENE_ACTOR_INIT_ACTOR_H
#define GUARD_SCENE_ACTOR_INIT_ACTOR_H

#include "scene_actor_state.h"
#include "scene_actor_init_primitives.h"

/* Callback slots have distinct indirect-call contracts. Write through the typed
 * slot so the compiler retains that contract even when the owner exposes a union. */
static inline void MonsterInit_WriteCallback(void **slot, void *callback)
{
    *slot = callback;
}

static inline void MonsterInit_WriteAnimationQuery(SceneActorAnimationQuery *slot,
                                                   SceneActorAnimationQuery query)
{
    *slot = query;
}

static inline void MonsterInit_WriteCommandTable(SceneActorCommandHandler const **slot,
                                                 SceneActorCommandHandler const *table)
{
    *slot = table;
}

static inline void MonsterInit_WriteDataPointer(const void **slot, const void *data)
{
    *slot = data;
}

static inline void MonsterInit_WriteHalfword(u16 *slot, u32 value)
{
    *slot = value;
}

/* Older actor views keep these dispatch slots opaque. The stored callback's
 * signature belongs to the operation that installs it. */
static inline void MonsterInit_WriteContextUpdate(void *storage, SceneActorContextUpdate callback)
{
    SceneActorContextUpdate *slot = storage;
    *slot = callback;
}

static inline void MonsterInit_WriteActorAction(void *storage, SceneActorAction callback)
{
    SceneActorAction *slot = storage;
    *slot = callback;
}

static inline void MonsterInit_WriteDefaultReaction(void *storage,
                                                    SceneActorDefaultReaction callback)
{
    SceneActorDefaultReaction *slot = storage;
    *slot = callback;
}

static inline void MonsterInit_WriteSequenceQuery(SceneActorSequenceQuery *slot,
                                                  SceneActorSequenceQuery query)
{
    *slot = query;
}

static inline void MonsterInit_WriteContactPredicate(SceneActorContactPredicate *slot,
                                                     SceneActorContactPredicate predicate)
{
    *slot = predicate;
}

#endif

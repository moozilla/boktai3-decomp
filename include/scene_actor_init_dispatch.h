#ifndef GUARD_SCENE_ACTOR_INIT_DISPATCH_H
#define GUARD_SCENE_ACTOR_INIT_DISPATCH_H

#include "scene_actor_init_actor.h"

extern SceneActorCommandHandler const gUnk_086108B0[];
void SceneActorReaction_SinkAndRemove(struct SceneActorRegistryResetView *, s32);

/* Install the actor result, sequence selector, action, and contact-predicate
 * callback families used by the pursuit-group initializers. */
static inline void MonsterInit_InstallSequenceCallbacks(
    struct SceneActorRegistryResetView *actor, SceneActorResultCallback result,
    SceneActorSequenceQuery sequence, SceneActorAction entry, SceneActorAction alternateEntry,
    SceneActorAction standing, SceneActorAction hidden, SceneActorAction reaction,
    SceneActorAction defeat, SceneActorDefaultReaction defaultReaction,
    SceneActorContactPredicate contactPredicate, void *targetQuery, void *damage, void *contact)
{
    MonsterInit_SetBehaviorFlags(actor, 0x2000);
    MonsterInit_WriteResultCallback(&actor->contextCallback, result);
    MonsterInit_WriteSequenceQuery(&actor->animationQuery.sequence, sequence);
    MonsterInit_WriteActorAction(&actor->entryTarget224, entry);
    MonsterInit_WriteActorAction(&actor->entryTarget228, alternateEntry);
    MonsterInit_WriteActorAction(&actor->savedTarget, standing);
    MonsterInit_WriteActorAction(&actor->reactionTarget230, reaction);
    MonsterInit_WriteActorAction(&actor->deathControl, defeat);
    MonsterInit_WriteActorAction(&actor->field220, hidden);
    MonsterInit_WriteDefaultReaction(&actor->defaultReaction, defaultReaction);
    MonsterInit_WriteContactPredicate(&actor->contactQuery.predicate, contactPredicate);
    MonsterInit_WriteDataPointer(&actor->commandTable.opaque, gUnk_086108B0);
    actor->callback248 = 0;
    actor->update26C = targetQuery;
    actor->field238 = SceneActorReaction_SinkAndRemove;
    actor->damageCallback = damage;
    actor->contactCallback = contact;
    actor->field240 = 0;
    MonsterInit_ClearMotion(actor);
}

#endif

#ifndef GUARD_SCENE_ACTOR_INIT_DISPATCH_H
#define GUARD_SCENE_ACTOR_INIT_DISPATCH_H

#include "scene_actor_init_actor.h"

extern SceneActorCommandHandler const gUnk_086108B0[];
void SceneActorReaction_SinkAndRemove(struct ChaseActor *, s32);

static inline void MonsterInit_InitializeCallbackDefaults(struct ChaseActor *actor,
                                                          void *targetQuery, void *damage,
                                                          void *contact)
{
    MonsterInit_WriteDataPointer(&actor->base.commandTable.opaque, gUnk_086108B0);
    actor->base.callback248 = 0;
    actor->base.update26C = targetQuery;
    actor->base.field238 = SceneActorReaction_SinkAndRemove;
    actor->base.damageCallback = damage;
    actor->base.contactCallback = contact;
    actor->base.field240 = 0;
    MonsterInit_ClearMotion(actor);
}

/* Install the actor result, sequence selector, action, and contact-predicate
 * callback families used by the pursuit-group initializers. */
static inline void MonsterInit_InstallSequenceCallbacks(
    struct ChaseActor *actor, SceneActorResultCallback result, SceneActorSequenceQuery sequence,
    SceneActorAction entry, SceneActorAction alternateEntry, SceneActorAction standing,
    SceneActorAction hidden, SceneActorAction reaction, SceneActorAction defeat,
    SceneActorDefaultReaction defaultReaction, SceneActorContactPredicate contactPredicate,
    void *targetQuery, void *damage, void *contact)
{
    MonsterInit_SetBehaviorFlags(actor, 0x2000);
    MonsterInit_WriteResultCallback(&actor->base.contextCallback, result);
    MonsterInit_WriteSequenceQuery(&actor->base.animationQuery.sequence, sequence);
    MonsterInit_WriteActorAction(&actor->base.entryTarget224, entry);
    MonsterInit_WriteActorAction(&actor->base.entryTarget228, alternateEntry);
    MonsterInit_WriteActorAction(&actor->base.savedTarget, standing);
    MonsterInit_WriteActorAction(&actor->base.reactionTarget230, reaction);
    MonsterInit_WriteActorAction(&actor->base.deathControl, defeat);
    MonsterInit_WriteActorAction(&actor->base.field220, hidden);
    MonsterInit_WriteDefaultReaction(&actor->base.defaultReaction, defaultReaction);
    MonsterInit_WriteContactPredicate(&actor->base.contactQuery.predicate, contactPredicate);
    MonsterInit_InitializeCallbackDefaults(actor, targetQuery, damage, contact);
}

static inline void MonsterInit_InitializeAnimationActions(struct ChaseActor *actor, void *result,
                                                          u32 (*animation)(void *), void *entry,
                                                          void *alternateEntry, void *standing,
                                                          void *hidden, void *reaction,
                                                          void *defeat, void *defaultReaction)
{
    MonsterInit_SetBehaviorFlags(actor, 0x2000);
    actor->base.contextCallback = result;
    MonsterInit_WriteAnimationQuery(&actor->base.animationQuery.word, animation);
    actor->base.entryTarget224 = entry;
    actor->base.entryTarget228 = alternateEntry;
    actor->base.savedTarget = standing;
    actor->base.reactionTarget230 = reaction;
    actor->base.deathControl = defeat;
    actor->base.field220 = hidden;
    actor->base.defaultReaction = defaultReaction;
}

static inline void
MonsterInit_InstallAnimationCallbacks(struct ChaseActor *actor, void *result,
                                      u32 (*animation)(void *), void *entry, void *alternateEntry,
                                      void *standing, void *hidden, void *reaction, void *defeat,
                                      void *defaultReaction, u8 (*contactQuery)(void *),
                                      void *targetQuery, void *damage, void *contact)
{
    MonsterInit_InitializeAnimationActions(actor, result, animation, entry, alternateEntry,
                                           standing, hidden, reaction, defeat, defaultReaction);
    MonsterInit_WriteContactPredicate(&actor->base.contactQuery.predicate, contactQuery);
    MonsterInit_InitializeCallbackDefaults(actor, targetQuery, damage, contact);
}

static inline void MonsterInit_InstallOpaqueContactCallbacks(
    struct ChaseActor *actor, void *result, u32 (*animation)(void *), void *entry,
    void *alternateEntry, void *standing, void *hidden, void *reaction, void *defeat,
    void *defaultReaction, void *contactQuery, void *targetQuery, void *damage, void *contact)
{
    MonsterInit_InitializeAnimationActions(actor, result, animation, entry, alternateEntry,
                                           standing, hidden, reaction, defeat, defaultReaction);
    MonsterInit_WriteCallback(&actor->base.contactQuery.opaque, contactQuery);
    MonsterInit_InitializeCallbackDefaults(actor, targetQuery, damage, contact);
}

#endif

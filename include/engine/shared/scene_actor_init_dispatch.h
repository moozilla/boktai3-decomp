#ifndef GUARD_SCENE_ACTOR_INIT_DISPATCH_H
#define GUARD_SCENE_ACTOR_INIT_DISPATCH_H

#include "engine/shared/scene_actor_init_actor.h"

extern SceneActorCommandHandler const gUnk_086108B0[];
void SceneActorReaction_SinkAndRemove(struct ChaseActor *, s32);

static inline void MonsterInit_InitializeCallbackDefaults(struct ChaseActor *actor,
                                                          void *targetQuery, void *damage,
                                                          void *contact)
{
    MonsterInit_WriteDataPointer(&actor->base.commandTable.opaque, gUnk_086108B0);
    actor->base.routeReset = 0;
    actor->base.targetQuery = targetQuery;
    actor->base.sinkCallback = SceneActorReaction_SinkAndRemove;
    actor->base.onDamage = damage;
    actor->base.onContact = contact;
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
    MonsterInit_WriteResultCallback(&actor->base.onUpdate, result);
    MonsterInit_WriteSequenceQuery(&actor->base.animationQuery.sequence, sequence);
    MonsterInit_WriteActorAction(&actor->base.onEntry, entry);
    MonsterInit_WriteActorAction(&actor->base.onAlternateEntry, alternateEntry);
    MonsterInit_WriteActorAction(&actor->base.onStanding, standing);
    MonsterInit_WriteActorAction(&actor->base.onReaction, reaction);
    MonsterInit_WriteActorAction(&actor->base.onDefeat, defeat);
    MonsterInit_WriteActorAction(&actor->base.onHidden, hidden);
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
    actor->base.onUpdate = result;
    MonsterInit_WriteAnimationQuery(&actor->base.animationQuery.word, animation);
    actor->base.onEntry = entry;
    actor->base.onAlternateEntry = alternateEntry;
    actor->base.onStanding = standing;
    actor->base.onReaction = reaction;
    actor->base.onDefeat = defeat;
    actor->base.onHidden = hidden;
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

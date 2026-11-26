#ifndef GUARD_SCENE_ACTOR_RESTORATION_H
#define GUARD_SCENE_ACTOR_RESTORATION_H
#include "scene/actor/scene_actor_registry.h"
extern const u8 gUnk_0824E128[];
/* The implementation ignores the forwarded table and index. */
void SceneActorReaction_ResetResponseState(void *, const void *, u32);
void SceneActorReaction_RestoreAfterDelay(struct ChaseActor *, s32);
#endif

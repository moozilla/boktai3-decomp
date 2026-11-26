#ifndef GUARD_SCENE_SCHEDULED_EFFECT_H
#define GUARD_SCENE_SCHEDULED_EFFECT_H
#include "engine/shared/scene_input_widget.h"
#include "engine/shared/engine.h"
#include "engine/shared/scene_actor_components.h"
#include "actors/task_4_e01/contact_swarm_sprite.h"
#include "engine/shared/contact_collider.h"
/* Four-slot animated-body effect used by the scheduling manager. */
struct SceneScheduledEffect {
    struct SceneInputWidget animation;
    struct SceneActorBody body;
    struct ContactSwarmSprite sprite;
    struct ContactPosition position;
    struct ContactCollider collider;
    u16 variant, animationIndex, active;
    s16 lifetime, delay, state, timer;
    s16 slot; /* -1 until allocated from the four-slot pool */
    u16 flags, unknown142;
};
struct SceneScheduledEffectPool {
    struct Task task;
    struct SceneInputResource resource;
    struct SceneInputResource *source;
    struct SceneScheduledEffect slots[4];
    u32 occupied;
};
struct SceneScheduledEffect *SceneScheduledEffect_Allocate(struct SceneScheduledEffectPool *);
void SceneScheduledEffect_LoadResource(struct SceneScheduledEffectPool *);
s32 SceneScheduledEffect_InitializeManager(struct SceneScheduledEffectPool *);
s32 StateSpritePool_Tick(struct SceneScheduledEffectPool *);
s32 AnimatedBodyPool_Destroy(struct SceneScheduledEffectPool *);
#endif

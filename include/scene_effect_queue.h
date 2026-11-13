#ifndef GUARD_SCENE_EFFECT_QUEUE_H
#define GUARD_SCENE_EFFECT_QUEUE_H
#include "scene_effect.h"
struct SceneEffectMessage {
    u32 id;
    u16 *data;
};
struct SceneEffectPostedMessage {
    u32 id : 16, reserved : 8, count : 8;
    s16 *values;
};
s32 SceneEffectQueue_UpdateEffects(struct SceneEffectQueue *);
s32 CallbackPool_Destroy(struct SceneEffectQueue *);
s32 SceneEffect_InitializeEmitterOptions(struct SceneEffectQueue *);
struct SceneEffectQueue *SceneEffectQueue_Create(u16);
void SceneEffectQueue_PostTypeZeroMessage(s32);
void SceneEffectQueue_PostTypeOneMessage(s32);
void SceneEffectQueue_PostTypeThreeMessage(s32);
s32 SceneEffectQueue_QueueTypeZeroEffect(void);
s32 SceneEffectQueue_QueueTypeTwoEffect(void);
s32 SceneEffectQueue_QueueTypeOneEffect(void);
#endif

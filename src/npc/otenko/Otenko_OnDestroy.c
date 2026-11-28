#include "npc/otenko/otenko_actor.h"
#include "engine/ram.h"

void SceneMessages_UnregisterReceiver(struct SceneEventReceiver *);
void ParticlePool_Destroy(u32);

s32 Otenko_OnDestroy(void *context, struct OtenkoActor *npc)
{
    struct OtenkoEffectState *effect = &npc->effect;
    SceneMessages_UnregisterReceiver(&npc->receiver);
    ParticlePool_Destroy(effect->particleHandle);
    if (effect->attached)
        GroundEffect_Destroy(&npc->groundEffect);
    gSceneNpc = 0;
}

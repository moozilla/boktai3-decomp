#include "npc/story_character/scene_character.h"

void ParticlePool_Destroy(u32);
void SceneMessages_UnregisterReceiver(struct SceneEventReceiver *);

s32 SceneCharacter_OnDestroy(void *context, struct SceneNpc *actor)
{
    if (actor->kind == 29)
        ParticlePool_Destroy(actor->effects.particleHandle);
    SceneMessages_UnregisterReceiver(&actor->receiver);
}

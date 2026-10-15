#include "scene_actor_attachment.h"
#define SceneActorEffect_PlayCategorySoundIfEnabled sub_0807C374
void SceneActorEffect_PlayCategorySoundIfEnabled(u8 gate, u8 event, u8 category, u8 variant)
{
    if (gate == 0)
        sub_0807C11C(event, category, variant);
}

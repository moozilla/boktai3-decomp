#include "scene_actor_attachment.h"
#define SceneActorEffect_PlayCategorySoundIfEnabled sub_0807C374
void SceneActorEffect_PlayCategorySoundIfEnabled(u8 gate, u32 inputEvent, u32 inputCategory,
                                                 u32 inputVariant)
{
    u8 event = inputEvent, category = inputCategory, variant = inputVariant;
    if ((u8)gate == 0)
        sub_0807C11C(event, category, variant);
}

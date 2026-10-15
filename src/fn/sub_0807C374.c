#include "scene_actor_attachment.h"
void SceneActorEffect_PlayCategorySoundIfEnabled(u8 gate, u32 inputEvent, u32 inputCategory,
                                                 u32 inputVariant)
{
    u8 event = inputEvent, category = inputCategory, variant = inputVariant;
    if ((u8)gate == 0)
        SceneActorEffect_PlayCategorySound(event, category, variant);
}

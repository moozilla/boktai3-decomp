#include "scene_actor_attachment.h"
void SceneActorEffect_PlayCategorySoundIfEnabled(u32 inputGate, u32 inputEvent, u32 inputCategory,
                                                 u32 inputVariant)
{
    u8 gate = inputGate;
    u8 event = inputEvent, category = inputCategory, variant = inputVariant;
    if ((u8)gate == 0)
        SceneActorEffect_PlayCategorySound(event, category, variant);
}

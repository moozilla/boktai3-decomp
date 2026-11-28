#include "scene/actor/scene_actor_attachment.h"
void SceneActorEffect_PlayCategorySoundIfEnabled(u32 gateArg, u32 eventArg, u32 categoryArg,
                                                 u32 variantArg)
{
    u8 gate = gateArg;
    u8 event = eventArg, category = categoryArg, variant = variantArg;

    if (gate == 0)
        SceneActorEffect_PlayCategorySound(event, category, variant);
}

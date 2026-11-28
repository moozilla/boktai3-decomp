#include "scene/actor/animation_apply.h"

void SceneActorAnimation_SelectExplicitVariant(struct RoomActor *actor, s32 index, u32 variant,
                                               u32 flags)
{
    struct SceneActorProfileEntry *entry =
        (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, index);
    SceneActorAnimation_Apply(actor, entry->value, variant, flags);
}

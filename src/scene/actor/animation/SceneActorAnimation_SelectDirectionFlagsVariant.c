#include "scene/actor/animation_apply.h"

void SceneActorAnimation_SelectDirectionFlagsVariant(struct RoomActor *actor, s32 index,
                                                     u32 variantArgument)
{
    struct SceneActorProfileEntry *entry =
        (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, index);
    u8 directionVariant, mirror;
    SceneActorAnimation_GetDirectionFrame(actor, &directionVariant, &mirror);
    SceneActorAnimation_Apply(actor, entry->value, variantArgument, mirror | entry->flags);
}

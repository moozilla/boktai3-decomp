#include "scene/actor/animation_apply.h"

void SceneActorAnimation_SelectDirectionalEntryFlags(struct RoomActor *actor, s32 index,
                                                     u32 extraFlags)
{
    struct SceneActorProfileEntry *entry =
        (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, index);
    u8 directionVariant, mirror;
    SceneActorAnimation_GetDirectionFrame(actor, &directionVariant, &mirror);
    SceneActorAnimation_Apply(actor, entry->value, directionVariant,
                              mirror | extraFlags | entry->flags);
}

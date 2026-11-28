#include "scene/actor/animation_apply.h"

void SceneActorAnimation_SelectEntryFlags(struct RoomActor *actor, s32 index, u32 extraFlags)
{
    struct SceneActorProfileEntry *entry =
        (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, index);
    SceneActorAnimation_Apply(actor, entry->value, 0, extraFlags | entry->flags);
}

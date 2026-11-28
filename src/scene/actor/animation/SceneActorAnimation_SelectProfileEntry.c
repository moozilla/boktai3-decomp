#include "scene/actor/animation_apply.h"

void SceneActorAnimation_SelectProfileEntry(struct RoomActor *actor, s32 index)
{
    struct SceneActorProfileEntry *entry =
        (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, index);
    SceneActorAnimation_Apply(actor, entry->value, entry->state, entry->flags);
}

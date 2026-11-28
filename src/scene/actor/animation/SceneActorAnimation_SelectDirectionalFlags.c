#include "scene/actor/animation_apply.h"

void SceneActorAnimation_SelectDirectionalFlags(struct RoomActor *actor, s32 index)
{
    struct SceneActorProfileEntry *entry =
        (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, index);
    u8 directionVariant, mirror;
    SceneActorAnimation_GetDirectionFrame(actor, &directionVariant, &mirror);
    SceneActorAnimation_Apply(actor, entry->value, 0, mirror | entry->flags);
}

#include "scene/actor/animation_apply.h"

/* SceneActorAnimation_SelectDirectionalExtraFlags: Selects a profile animation with variant zero and combines direction-derived mirroring with entry flags and caller-supplied playback flags */
void SceneActorAnimation_SelectDirectionalExtraFlags(struct RoomActor *actor, s32 index,
                                                     u32 extraFlags)
{
    struct SceneActorProfileEntry *entry =
        (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, index);
    u8 directionVariant, mirror;
    SceneActorAnimation_GetDirectionFrame(actor, &directionVariant, &mirror);
    SceneActorAnimation_Apply(actor, entry->value, 0, mirror | extraFlags | entry->flags);
}

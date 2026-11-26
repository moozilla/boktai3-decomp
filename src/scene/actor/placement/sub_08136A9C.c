#include "scene/actor/scene_actor_status.h"
#include "scene/motion/scene_actor_chase_shared.h"
#include "global.h"
#include "scene/actor/scene_actor_placement.h"
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void Sound_PlaySong(u32);

static inline void SetFlags(struct RoomActor *s, u16 mask)
{
    s->motionFlags = mask | s->motionFlags;
}
s32 sub_08136A9C(struct RoomActor *s)
{
    struct SceneActorChaseTracking *tracking;
    SetFlags(s, 0x20);
    SceneActorAttachment_ReplaceResponseSlot(s, 3, 0, s->category);
    tracking = &s->tracking;
    ActorStatus_ClearTrackingFlags(tracking, 8);
    SceneActorChase_AndWordFlags(&s->statusFlags, ~0x1000000);
    Sound_PlaySong(0x13e);
    return 1;
}

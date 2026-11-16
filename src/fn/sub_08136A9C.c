#include "global.h"
#include "scene_actor_placement.h"
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void Sound_PlaySong(u32);

static inline void SetFlags(struct RoomActor *s, u16 mask)
{
    s->flags2D4 = mask | s->flags2D4;
}
static inline void ClearFlags32(u32 *p, u32 mask)
{
    *p &= mask;
}
static inline void ClearTrackingFlags(struct SceneActorChaseTracking *tracking, s32 mask)
{
    tracking->flags &= mask;
}
s32 sub_08136A9C(struct RoomActor *s)
{
    struct SceneActorChaseTracking *tracking;
    SetFlags(s, 0x20);
    SceneActorAttachment_ReplaceResponseSlot(s, 3, 0, s->category);
    tracking = &s->tracking;
    ClearTrackingFlags(tracking, ~8);
    ClearFlags32(&s->flags2D0, ~0x1000000);
    Sound_PlaySong(0x13e);
    return 1;
}

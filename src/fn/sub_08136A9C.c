#include "global.h"
#include "scene_actor_placement.h"
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void Sound_PlaySong(u32);

static inline void SetFlags(struct SceneActorPlacementView *s, u16 mask)
{
    s->flags2D4 = mask | s->flags2D4;
}
static inline void ClearFlags32(u32 *p, u32 mask)
{
    *p &= mask;
}
static inline void ClearByte(u8 *b, s32 mask)
{
    b[0xf] &= mask;
}
s32 sub_08136A9C(struct SceneActorPlacementView *s)
{
    u8 *block;
    SetFlags(s, 0x20);
    SceneActorAttachment_ReplaceResponseSlot(s, 3, 0, s->category);
    block = &s->unknown9E[0x3e];
    ClearByte(block, ~8);
    ClearFlags32(&s->flags2D0, ~0x1000000);
    Sound_PlaySong(0x13e);
    return 1;
}

#include "global.h"
#include "scene_actor_placement.h"

static inline void SetFlags(struct SceneActorPlacementView *s, u16 mask)
{
    s->flags2D4 = mask | s->flags2D4;
}
static inline void ClearFlags32(u32 *p, u32 mask)
{
    *p &= mask;
}
s32 Lun_PlayResponseAndResetState(struct SceneActorPlacementView *s)
{
    if (!s->field2E4)
        Sound_PlaySong(0x211);
    else
        Sound_PlaySong(0x13E);
    if (*(u32 *)((u8 *)s + 0x2DC) & 0x40000)
        SetFlags(s, 0x20);
    {
        const void *control = s->savedControl230;
        u8 code = 0x12;
        u32 zero;
        u8 one;
        u8 zero8;
        u32 *flags;
        u8 *taken;
        u32 three;
        taken = &s->field2B2;
        zero = 0;
        one = 1;
        *taken = one;
        s->field2B0 = zero;
        s->field2B5 = code;
        s->control = control;
        zero8 = 0;
        s->field2C8 = zero;
        flags = &s->flags114;
        {
            u32 mask = -2;
            *flags &= mask;
        }
        s->field104 = zero;
        three = 3;
        s->state2A8 = three;
        s->phase2A9 = zero8;
        s->behavior2AA = zero8;
        s->counter2AC = zero;
        s->field2B1 = one;
        ClearFlags32(&s->flags2D0, 0xFEFFFFFF);
        SceneActorPlacement_ClearFlags(&s->flags2D6, 0xFFFFF636);
    }
    return 1;
}

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
static inline u32 MaskFlags(u16 *p, u32 mask)
{
    mask &= *p;
    *p = mask;
    return mask;
}
static inline u16 TestFlags(u32 value, u32 mask)
{
    return mask & value;
}
s32 SceneActorPlacement_PlayResponseAndResetState(struct SceneActorPlacementView *s)
{
    u8 new_var;
    u32 flags = MaskFlags(&s->flags2D4, 0xFFFFFEFF);
    switch (s->category) {
        case 0x12:
            if (!s->field2E4)
                goto fallback;
            Sound_PlaySong(0x108);
            break;
        case 0x13:
            if (!s->field2E4)
                goto fallback;
            Sound_PlaySong(0x109);
            break;
        case 0x14:
            if (!s->field2E4)
                goto fallback;
            Sound_PlaySong(0x13E);
            break;
        case 0x17:
            if (!s->field2E4) {
            fallback:
                Sound_PlaySong(0x211);
            } else if ((new_var = TestFlags(flags, 8))) {
                Sound_PlaySong(0x13E);
                Sound_PlaySong(0x12A);
            } else {
                Sound_PlaySong(0x13E);
            }
            break;
    }
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
        SceneActorPlacement_ClearFlags(&s->flags2D6, 0xFFFFF636);
        ClearFlags32(&s->flags2D0, 0xFEFFFFFF);
    }
    return 1;
}

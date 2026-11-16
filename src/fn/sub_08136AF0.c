#include "global.h"
#include "scene_actor_placement.h"

static inline void SetFlags(struct RoomActor *s, u16 mask)
{
    s->motionFlags = mask | s->motionFlags;
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
s32 SceneActorPlacement_PlayResponseAndResetState(struct RoomActor *s)
{
    u8 new_var;
    u32 flags = MaskFlags(&s->motionFlags, 0xFFFFFEFF);
    switch (s->category) {
        case 0x12:
            if (!s->reactionSoundVariant)
                goto fallback;
            Sound_PlaySong(0x108);
            break;
        case 0x13:
            if (!s->reactionSoundVariant)
                goto fallback;
            Sound_PlaySong(0x109);
            break;
        case 0x14:
            if (!s->reactionSoundVariant)
                goto fallback;
            Sound_PlaySong(0x13E);
            break;
        case 0x17:
            if (!s->reactionSoundVariant) {
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
        const void *control = s->onReaction;
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
        s->stepDone = zero;
        s->controlKind = code;
        s->target = control;
        zero8 = 0;
        s->field2C8 = zero;
        flags = &s->pendingControlFlags;
        {
            u32 mask = -2;
            *flags &= mask;
        }
        s->pendingControl = zero;
        three = 3;
        s->state2A8 = three;
        s->phase2A9 = zero8;
        s->behavior2AA = zero8;
        s->counter2AC = zero;
        s->field2B1 = one;
        SceneActorPlacement_ClearFlags(&s->reactionFlags, 0xFFFFF636);
        ClearFlags32(&s->statusFlags, 0xFEFFFFFF);
    }
    return 1;
}

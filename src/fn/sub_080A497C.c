
#ifdef NONMATCHING
// fuzzy: 80.2%
// foundry draft (worker best 80.2%); what differs is mostly register allocation and instruction order
#define SceneActorPlacement_BeginDefaultResponse sub_080A497C
#include "scene_actor_reset.h"
s32 sub_080A497C(struct SceneActorRegistryResetView *p)
{
    u32 offset;
    u32 control;
    u32 zero;
    u8 byteZero;
    u32 one;
    u16 stateOffset;
    if (p->reactionSoundVariant == 0)
        Sound_PlaySong(0x211);
    else
        Sound_PlaySong(0x112);
    if (p->field2DC & 0x40000) {
        u32 mask = 32;
        u16 *flags = &p->flags2D4;
        mask |= *flags;
        *flags = mask;
    }
    offset = 0x230;
    control = *(u32 *)((u8 *)p + offset);
    {
        u32 kind = 18;
        u8 *field = &p->field2B2;
        zero = 0;
        one = 1;
        *field = one;
        offset += 0x80;
        *((u8 *)p + offset) = zero;
        offset += 5;
        *((u8 *)p + offset) = kind;
        p->target = (void *)control;
        {
            u16 *field;
            u16 halfOffset = 0x2C8;
            field = (u16 *)((u8 *)p + halfOffset);
            byteZero = 0;
            *field = zero;
        }
        {
            u32 *flags = &p->flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        p->word104 = zero;
        {
            u8 state = 3;
            u32 stateOffset2;
            u32 phaseOffset;
            stateOffset = 0x2A8;
            *((u8 *)p + stateOffset) = state;
            phaseOffset = 0x2A9;
            *((u8 *)p + phaseOffset) = byteZero;
            stateOffset += 2;
            *((u8 *)p + stateOffset) = byteZero;
            offset = 0x2AC;
            *(u32 *)((u8 *)p + offset) = zero;
            phaseOffset += 8;
            {
                u8 *field = (u8 *)p + phaseOffset;
                *field = one;
            }
        }
    }
    {
        u32 *flags = &p->flags2D0;
        u32 mask = ~0x1000000;
        *flags &= mask;
    }
    offset += 0x2A;
    {
        u16 *flags = (u16 *)((u8 *)p + offset);
        s32 mask = (s32)0xFFFFF636;
        mask &= *flags;
        *flags = mask;
    }
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080A497C);
#endif

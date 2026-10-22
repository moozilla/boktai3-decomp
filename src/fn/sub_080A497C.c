// MATCHING_CONCESSION: register-pinning
#define SceneActorPlacement_BeginDefaultResponse sub_080A497C
#include "scene_actor_reset.h"

static inline u32 TestResponse(struct SceneActorRegistryResetView *p)
{
    register u32 off asm("r2") = 0x2DC;
    register u32 *field asm("r0");
    asm("" : "+r"(off));
    field = (void *)((u8 *)p + off);
    asm("" : "+r"(field));
    return *field & 0x40000;
}
s32 SceneActorPlacement_BeginDefaultResponse(struct SceneActorRegistryResetView *p)
{
    u32 offset;
    u32 control;
    u32 zero;
    u8 byteZero;
    u32 one;
    register u32 stateOffset asm("r2");
    if (p->reactionSoundVariant == 0)
        Sound_PlaySong(0x211);
    else
        Sound_PlaySong(0x112);
    if (TestResponse(p)) {
        register u32 mask asm("r0") = 32;
        register u32 off asm("r3") = 0x2D4;
        register u16 *flags asm("r2");
        asm("" : "+r"(off));
        flags = (void *)((u8 *)p + off);
        asm("" : "+r"(flags), "+r"(mask));
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
            register u16 *field asm("r0");
            register u32 halfOffset asm("r2") = 0x2C8;
            asm("" : "+r"(halfOffset));
            field = (u16 *)((u8 *)p + halfOffset);
            asm("" : "+r"(field));
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
            register u32 state asm("r1") = 3;
            u32 stateOffset2;
            register u32 phaseOffset asm("r1");
            asm("" : "+r"(state));
            stateOffset = 0x2A8;
            *((u8 *)p + stateOffset) = state;
            asm("" : "+r"(stateOffset));
            phaseOffset = 0x2A9;
            asm("" : "+r"(phaseOffset));
            *((u8 *)p + phaseOffset) = byteZero;
            stateOffset += 2;
            { register u8 *field asm("r0") = (u8 *)p + stateOffset;
              asm("" : "+r"(field));
              *field = byteZero; }
            offset = 0x2AC;
            *(u32 *)((u8 *)p + offset) = zero;
            phaseOffset += 8;
            {
                register u8 *field asm("r0") = (u8 *)p + phaseOffset;
                asm("" : "+r"(field));
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

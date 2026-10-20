#include "global.h"
#define PairedSceneActor_InitHalves sub_081B6354
void ActorStatus_InitializeCollider(void *, s32, s32, s32);
struct Half {
    u8 pad0[0x98];
    u8 collider[0xac];
    u16 w144;
    u16 w146;
    u16 w148;
    u8 pad14a[0xc0];
    u16 w20a;
    u8 pad20c[2];
    u16 w20e;
    u8 pad210[0xc];
};
#ifdef NONMATCHING
// fuzzy: 94.9%
// Only the placement of the 'down' computation relative to pointer setup differs.
void PairedSceneActor_InitHalves(struct Half *base, s32 k, s32 a2, s32 a3, s32 s1, s32 s2, s32 s3,
                                 s32 s3b, s32 s4)
{
    u32 down, up;
    s32 i;
    i = 0;
    k &= 0x7f;
    up = (0xa0 + k) & 0xff;
    down = (0xa0 - k) & 0xff;
    do {
        if (i == 1)
            *(u16 *)((u8 *)base + 0x360) = up;
        else
            do {
                base[i].w144 = down;
            } while (0);
        base[i].w146 = a2;
        base[i].w20a = s1;
        ActorStatus_InitializeCollider(base[i].collider, s1, s2, s3);
        *(u16 *)((u8 *)&base[i] + 0x220) = s3b;
        *(u16 *)((u8 *)&base[i] + 0x20e) = a3;
        base[i].w148 = s4;
        i++;
    } while (i <= 1);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081B6354);
#endif

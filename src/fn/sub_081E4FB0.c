#include "global.h"
// MATCHING_CONCESSION: register-pinning
/* Script command: configure the singleton animation controller. */
#define H(o)     (*(u16 *)((u8 *)g + (o)))
#define B(o)     (*(u8 *)((u8 *)g + (o)))
#define W(o)     (*(u32 *)((u8 *)g + (o)))
#define OWNER(o) (*(u16 *)(*(u8 **)((u8 *)g + 0x18) + (o)))
extern u8 *gUnk_020003C8;
u8 *sub_081EA730(s32, s32);
struct G {
    u8 f0[0xE2C];
    u16 e2c;
    u8 f1[3];
    u16 e32;
    u8 f2[0xF5A - 0xE34];
    u16 xa : 4;
    u16 xb : 12;
    u16 b;
    u8 f3[0xF64 - 0xF5E];
    u8 c;
    u8 f4[0xF86 - 0xF65];
    u8 d;
    u8 e;
};
s32 Script_SeekToKeyword(s32);
void Actor_ProjectAndClampToScreen(void *);
s32 Script_GetValue(void);
s32 sub_0821ABA8(s32, s32);
static inline u32 mode_bits(u8 *p, u32 mask)
{
    register u32 v asm("r1") = *p;
    return mask & v;
}
void sub_081E4FB0(void)
{
    struct G *g = (struct G *)gUnk_020003C8;
    u32 three = 2;
    if (g == 0) {
        g = (struct G *)sub_081EA730(0xBB34, 0);
        if (g == 0)
            return;
    }
    W(0xf50) = sub_0821ABA8(0x70, 0);
    if (Script_SeekToKeyword(0x74)) {
        H(0xed0) = Script_GetValue();
        H(0xed2) = Script_GetValue();
    } else {
        H(0xed2) = 0;
        H(0xed0) = 0;
    }
    g->b = g->xa;
    {
        u32 mode = 2;
        g->xa = mode;
    }
    {
        register u32 raw asm("r1") = *(u16 *)((u8 *)g + 0xf5a);
        register u32 mask asm("r2") = 15;
        *(u16 *)((u8 *)g + 0xf5a) = (mask & raw) | 0x90;
        if (g->b == 3) {
            register u32 bits asm("r0") = mode_bits((u8 *)g + 0xf5a, mask);
            if (bits == 3) {
                g->d = g->e2c;
                g->e = g->e32;
            }
        }
    }
    {
        u8 one = 1;
        g->c |= one;
    }
}

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
void sub_08220C8C(void *, u32, u32, u32, u32);
s32 Script_SeekToKeyword(s32);
void sub_081E3B6C(void *);
s32 Script_GetValue(void);
s32 sub_0821ABA8(s32, s32);
static inline u32 mode_bits(u8 *p, u32 mask)
{
    register u32 v asm("r1") = *p;
    return mask & v;
}
void sub_081E5098(void)
{
    struct G *g = (struct G *)gUnk_020003C8;
    u32 three = 2;
    if (g == 0) {
        g = (struct G *)sub_081EA730(0xBB34, 0);
        if (g == 0)
            return;
    }
    H(0xf8a) = Script_GetValue();
    {
        register u32 shifted asm("r0") = sub_0821ABA8(0x64, 0) << 24;
        u32 flip = 0;
        u32 mask = 0x07000000;
        u8 direction = (mask & shifted) >> 24;
        B(0xf68) = direction;
        if (direction > 4) {
            direction = 8 - direction;
            flip = 1;
        }
        B(0xf66) = direction;
        B(0xf6b) = flip;
    }
    sub_08220C8C((u8 *)g + 0xe24, W(0xe74), 0, B(0xf66), B(0xf6b));
    g->b = g->xa;
    {
        u32 mode = 2;
        g->xa = mode;
    }
    {
        register u32 raw asm("r1") = *(u16 *)((u8 *)g + 0xf5a);
        register u32 mask asm("r2") = 15;
        *(u16 *)((u8 *)g + 0xf5a) = (mask & raw) | 0;
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

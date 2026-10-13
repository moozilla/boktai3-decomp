#include "global.h"
#ifdef NONMATCHING
// fuzzy: 75.0%
// 5 differing instruction runs; size -4 bytes; first diff at +0x4 (ldr r0, [pc, #8] vs ldr r0, [pc, #0x24])
struct Context {
    u8 pad[0x98];
    s32 values[16];
};
extern struct Context *gUnk_030025FC;
u8 sub_0821E52C(u16, void *);
u8 sub_0822B5FC(void *point)
{
    struct Context *ctx = gUnk_030025FC;
    if (ctx) {
        s32 i = 0;
        s32 *p = ctx->values;
        do {
            if (*p > 0 && sub_0821E52C((u16)*p, point))
                return 1;
            p++;
            i++;
        } while (i <= 15);
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0822B5FC);
#endif

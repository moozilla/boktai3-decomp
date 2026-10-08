#include "global.h"
struct T { u8 p[0xc]; u8 *f0c; };
extern struct T gUnk_02000438;
extern const u8 gUnk_08614184[];
u8 *sub_0821A66C(u8 *, u32 *);
void sub_0821AFB4(u8 *, const u8 *);
void sub_0821B004(void)
{
    u32 x;
    sub_0821AFB4(sub_0821A66C(gUnk_02000438.f0c, &x), gUnk_08614184);
}

#include "global.h"
struct S { u32 f0; u32 f4; const u8 *f8; };
extern struct S gUnk_02000428;
extern const u8 gUnk_0861418C[];
s32 Div(s32, s32);
u32 sub_0821ABE8(struct S *);
void sub_0821B90C(void)
{
    gUnk_02000428.f0 = 0;
    gUnk_02000428.f4 = Div(0x30, 8);
    gUnk_02000428.f8 = gUnk_0861418C;
    sub_0821ABE8(&gUnk_02000428);
}

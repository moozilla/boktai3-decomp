#include "global.h"
struct S { u8 filler[0x84]; s16 a; s16 b; };
extern struct S *gUnk_020004B4;
void sub_0811D270(s32, s32, s32, s32, s32);
void sub_0810ECEC(void)
{
    struct S *s = gUnk_020004B4;
    if (s == 0) {
        sub_0811D270(0, 0x64, 0, 0x12, (s32)s);
        sub_0811D270(0, 0x64, 0x12, 0x12, (s32)s);
    } else {
        sub_0811D270(s->a, 0x64, 0, 0x12, 0);
        sub_0811D270(s->b, 0x64, 0x12, 0x12, 0);
    }
}

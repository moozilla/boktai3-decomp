#include "palette_controller.h"

s32 sub_0821ABA8(u32, u32);
void Palette_ReplaceResource(s32, s32, s32, s32);
void sub_08003540(void)
{
    if (gUnk_02000468 != 0) {
        s32 a = sub_0821ABA8(0x73, 0);
        s32 b = sub_0821ABA8(0x69, 0);
        s32 c = sub_0821ABA8(0x6e, 0);
        s32 d = sub_0821ABA8(0x66, 0);
        Palette_ReplaceResource(a, b, c, d);
    }
}

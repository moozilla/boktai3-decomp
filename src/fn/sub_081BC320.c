#include "global.h"
void sub_080311B0(u32);
void sub_08030C84(u32);
void sub_08030F20(s32, s32, s32, s32);
void sub_08030B78(s32);
void sub_081BC320(u8 *p)
{
    sub_080311B0(0x2E9);
    sub_08030C84(*(u32 *)(p + 0x8E8));
    sub_08030F20(1, 0xf, 0x1c, 4);
    sub_08030B78(1);
}

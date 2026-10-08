#include "global.h"

void sub_082161B4(s32, s32, s32, s32, s32, s32, u32 *);
void sub_082164AC(s32, s32, s32);
void sub_0821656C(s32, s32, s32, s32, s32);

void sub_08163F14(s32 a, s32 b, s32 c, s32 d, u32 e)
{
    u32 z = 0;
    sub_082161B4(0, (s16)c, b, 0, 0, 1, &z);
    sub_082164AC(0, b, d);
    sub_0821656C(0, 0, 0, 0, 0);
    CpuSet(e, 0x03005160, 0x04000018);
}

#include "global.h"
void sub_0821FEB4(void *, u32, s32, s32, s32, s32, s32);
void sub_0821FF84(void *, s32, s32);
void sub_0821FE40(void *);
void sub_0821FF24(void *, s32, s32);
void sub_0821FF7C(void *, s32, s32, s32);
void sub_0818AEC8(void *a, u16 b, s32 c, s32 d, s32 e, s32 f, s32 g, s32 h)
{
    sub_0821FEB4(a, b, 0x4101, 0, 0x10, f, e);
    if (g != 0) sub_0821FF84(a, g, h);
    sub_0821FE40(a);
    sub_0821FF24(a, d, 0);
    sub_0821FF7C(a, c, 0x100, 0);
}

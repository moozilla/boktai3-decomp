#include "global.h"

extern void *gUnk_02000244;
void *sub_081A4EEC(s32, s32, s32);
void sub_081A4984(s32, s32, s32);

void sub_081A4F48(s32 a, s32 b, s32 c)
{
    if (gUnk_02000244 == 0)
        gUnk_02000244 = sub_081A4EEC(0, 0, 0);
    sub_081A4984(a, b, c);
}

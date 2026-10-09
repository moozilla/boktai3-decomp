#include "global.h"
void sub_08109480(u32, s32 *, s32 *, s32 *, s32 *);
void sub_0823ACD0(u8 *p)
{
    s32 values[4];
    sub_08109480((*(u8 **)(p + 0x344))[0x3d], &values[0], &values[1], &values[2], &values[3]);
    {
        s32 v = values[1];
        *(u16 *)(p + 0x41c) = v;
    }
    {
        s32 v = values[2];
        *(u16 *)(p + 0x41e) = v;
    }
    {
        s32 v = values[0];
        *(u16 *)(p + 0x420) = v;
    }
}

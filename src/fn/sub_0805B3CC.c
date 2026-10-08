#include "global.h"
extern u32 gUnk_02000580;
s32 sub_0805B2CC(void);
void sub_0815B15C(u32, u32, s32, u32, u32);
void sub_08057960(u8 *, u32);
void sub_0805B3CC(u8 *p)
{
    if (sub_0805B2CC() != 0) {
        u32 z;
        sub_0815B15C(gUnk_02000580, 0, -1, 0, z = 0);
        *(u16 *)(p + 0xc16) = z;
        sub_08057960(p, 1);
    }
}

#include "global.h"

extern u32 gUnk_02000560;
void sub_08021AA8(u32);
void sub_0802AAD4(u32 a)
{
    u32 m = 4;
    if ((gUnk_02000560 & m) == 0)
        sub_08021AA8(a);
}

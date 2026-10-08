#include "global.h"

extern u32 gUnk_02000160;
void sub_08069F30(void);

s32 sub_08069F34(u32 v)
{
    sub_08069F30();
    gUnk_02000160 = v;
    return 0;
}

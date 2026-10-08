#include "global.h"

extern u32 gUnk_02000580[];
s32 sub_0815924C(u8 *);

s32 sub_0815926C(s32 idx)
{
    return sub_0815924C((u8 *)gUnk_02000580[idx]);
}

#include "global.h"

extern const u16 gUnk_08E61280[];

u32 sub_0816CE8C(s32 i)
{
    return gUnk_08E61280[i * 8] & 15;
}

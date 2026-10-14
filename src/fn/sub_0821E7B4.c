#include "global.h"

u32 sub_0821E7B4(u8 *p, s32 i)
{
    return (p[i * 8 + 6] & 15) << 8;
}

#include "global.h"
#include "player.h"
static inline s32 Sub256(s32 x)
{
    return x - 256;
}
s32 sub_081FB7D8(u32 x)
{
    s32 v = (*(u8 **)gPlayers)[0x4f4];
    s32 r;
    if (v >= (s32)x)
        r = v - x;
    else
        r = v - Sub256(x);
    return r;
}

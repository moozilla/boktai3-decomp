#include "global.h"

struct Blk {
    u32 w[8];
};

s32 ResourceHeader_Relocate(struct Blk *dst, struct Blk *src)
{
    *dst = *src;
    dst->w[3] += (u32)src;
    dst->w[4] += (u32)src;
    dst->w[5] += (u32)src;
    dst->w[6] += (u32)src;
    dst->w[7] += (u32)src;
    return 0;
}

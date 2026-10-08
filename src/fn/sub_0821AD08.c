#include "global.h"
u8 *sub_0821ACDC(u32, u32 *);
u32 sub_0821A66C(u8 *, u32 *);
u32 sub_0821AFD8(u32, u32, u32);
u32 sub_0821AD08(u32 id, u32 b)
{
    u32 x, y;
    return sub_0821AFD8(sub_0821A66C(sub_0821ACDC(id, &x), &y), b, x);
}

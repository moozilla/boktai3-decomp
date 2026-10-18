#include "global.h"
/* Path_GetPointHeightOffset: Extracts the point's low attribute nibble as a fixed-point height offset tentatively */
#define Path_GetPointHeightOffset sub_0821E7B4

u32 sub_0821E7B4(u8 *p, s32 i)
{
    return (p[i * 8 + 6] & 15) << 8;
}

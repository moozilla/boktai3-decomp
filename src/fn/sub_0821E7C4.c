#include "global.h"
/* Path_GetPointActionKind: Extracts the high attribute nibble used to select point arrival behavior */
#define Path_GetPointActionKind sub_0821E7C4

u32 sub_0821E7C4(u8 *p, s32 i)
{
    return p[i * 8 + 6] >> 4;
}

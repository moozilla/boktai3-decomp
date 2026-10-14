#include "global.h"

u32 sub_0804271C(u32 *p, u32 mask)
{
    if (*p == 0)
        return 0;
    return **(u32 **)((u8 *)p + 0x90) & mask;
}

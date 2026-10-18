#include "global.h"

u32 RegistryF3E4_GetStateResult(u8 *p)
{
    u32 r;
    if (p)
        r = *(u32 *)(p + 0xd8);
    else
        r = 0;
    return r;
}

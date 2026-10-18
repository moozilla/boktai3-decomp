#include "global.h"
u8 Vanargand_IsEitherComponentFlag40Set(u8 *p)
{
    u32 m = 0x40;
    if ((*(u32 *)(p + 0x1f8) & m) || (*(u32 *)(p + 0x414) & m))
        return 1;
    return 0;
}

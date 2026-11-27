#include "global.h"
void MapScreen_HideDirectionalIndicators(u8 *s)
{
    *(u32 *)(s + 0x38E8) |= 1;
    *(u32 *)(s + 0x3948) |= 1;
    *(u32 *)(s + 0x39A8) |= 1;
    *(u32 *)(s + 0x3A08) |= 1;
}

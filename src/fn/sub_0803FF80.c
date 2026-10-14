#include "global.h"

extern const u8 gUnk_086058E8[];
void sub_082279A8(u32, u32, u32, u32, u32, u32, u32);

void ModifierMenu_BeginClose(u8 *p)
{
    *(u32 *)(p + 0x18) = 3;
    *(u32 *)(p + 0x30) = 0;
    if (*(s8 *)(p + 0x20) >= gUnk_086058E8[*(u32 *)(p + 0x1c)])
        *(s8 *)(p + 0x20) = 0;
    sub_082279A8(1, 5, 4, 4, 4, 0xFFFF, 0);
}

#include "global.h"
u32 TextWindow_Create(u32, u32, u32, u32);
void LinkScorePanel_CreateTextWindows(u8 *s)
{
    u32 v;
    *(u32 *)(s + 0x1004) = TextWindow_Create(1, 3, 10, 2);
    *(u32 *)(s + 0x1008) = TextWindow_Create(14, 12, 15, 2);
    *(u32 *)(s + 0x100C) = TextWindow_Create(14, 15, 15, 2);
    *(u32 *)(s + 0x1010) = TextWindow_Create(14, 17, 15, 2);
    *(u32 *)(s + 0x1014) = TextWindow_Create(14, 3, 15, 2);
    *(u32 *)(s + 0x1018) = TextWindow_Create(18, 6, 2, 2);
    *(u32 *)(s + 0x101C) = TextWindow_Create(26, 6, 2, 2);
    v = TextWindow_Create(18, 8, 8, 2);
    s += 0x1020;
    *(u32 *)s = v;
}

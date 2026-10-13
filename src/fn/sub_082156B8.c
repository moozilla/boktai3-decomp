#include "global.h"

void sub_08218CB0(void);
u32 Video_GetBackgroundMap(u32);
void Memory_Clear(u32, u32);

void Bg_ClearTilemap(u32 n)
{
    if (n == 0)
        sub_08218CB0();
    Memory_Clear(Video_GetBackgroundMap(n), 0x800);
}

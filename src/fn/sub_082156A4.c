#include "global.h"

void Memory_Clear(u32, u32);

void Bg_ClearAllTilemaps(void)
{
    Memory_Clear(0x02037000, 0x2000);
}

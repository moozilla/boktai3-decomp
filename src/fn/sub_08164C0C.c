#include "global.h"
#include "engine.h"

u16 *Hud_GetTilemapCell(u32, u32, u32);
void ResultPanel_ClearFiveTiles(void)
{
    if (!(gEngineFlags & 0x1000)) {
        u16 *tiles = Hud_GetTilemapCell(0, 25, 0);
        s32 i;
        for (i = 0; i < 5; i++)
            *tiles++ = 0xD000;
    }
}

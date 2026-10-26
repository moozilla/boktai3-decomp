#include "global.h"
#define StatDisplay_GetTileAddress Hud_GetTilemapCell
#define gDisplayFlags              gEngineFlags
extern u32 gDisplayFlags;
u16 *StatDisplay_GetTileAddress(u32, s32, s32);
void StatDisplay_DrawTwoDigitCounter(s32 value)
{
    u16 *tiles, *ones;
    s32 tens, remaining;
    if (!(gDisplayFlags & 0x1000)) {
        tiles = StatDisplay_GetTileAddress(0, 25, 0);
        *tiles++ = 0xD0C9;
        *tiles++ = 0xD0CA;
        *tiles++ = 0xD0CB;
        remaining = value;
        if (remaining > 99)
            remaining = 99;
        if (remaining <= 0) {
            tiles[0] = 0xD0CC;
            tiles[1] = 0xD0CC;
        } else {
            tens = 0;
            ones = tiles + 1;
            while (remaining > 9) {
                tens++;
                remaining -= 10;
            }
            tiles[0] = (tens + 0x11) | -12288;
            *ones = (remaining + 0x11) | -12288;
        }
    }
}

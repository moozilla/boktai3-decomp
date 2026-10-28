#include "global.h"
#include "render.h"
// Provisional name: Presentation_ClearTilemap
// Reset the presentation's 32 by 32 tilemap to its blank tile.
void Presentation_ClearTilemap(void)
{
    u16 *dst = gBgRecords[0].map;
    s32 y, x;
    for (y = 0; y < 32; y++)
        for (x = 0; x < 32; x++)
            *dst++ = 0xf001;
}

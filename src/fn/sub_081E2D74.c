#include "global.h"
#define Presentation_ClearTilemap sub_081E2D74
// Provisional name: Presentation_ClearTilemap
struct Tilemap {
    u8 pad[0x2c];
    u16 *tiles;
};
extern struct Tilemap gUnk_03004C30;
// Reset the presentation's 32 by 32 tilemap to its blank tile.
void Presentation_ClearTilemap(void)
{
    u16 *dst = gUnk_03004C30.tiles;
    s32 y, x;
    for (y = 0; y < 32; y++)
        for (x = 0; x < 32; x++)
            *dst++ = 0xf001;
}

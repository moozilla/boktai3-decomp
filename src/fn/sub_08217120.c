#include "global.h"
#include "render.h"
/* ObjShape_InitTables: Fills the sprite shape width/height tables (pixels, tiles, area) and the per-index attribute words from the shape size table */

extern const u16 gUnk_086140EC[];

void ObjShape_InitTables(void)
{
    s32 i;
    for (i = 0; i < 16; i++) {
        u32 e = gUnk_086140EC[i];
        s32 lo = *(const u8 *)&gUnk_086140EC[i];
        s32 hi = e >> 8;
        s32 tw = lo >> 3;
        s32 th = hi >> 3;
        gSpriteWidthPixels[i] = lo;
        gSpriteHeightPixels[i] = hi;
        gSpriteWidthTiles[i] = tw;
        gSpriteHeightTiles[i] = th;
        gSpriteTileArea[i] = tw * th;
        gSpriteShapeAttr[i] = ((i & 3) << 14) | ((i & 12) << 28);
    }
}

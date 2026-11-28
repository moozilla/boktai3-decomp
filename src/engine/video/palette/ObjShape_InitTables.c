#include "global.h"
#include "engine/video/render.h"
/* ObjShape_InitTables: Fills the sprite shape width/height tables (pixels, tiles, area) and the per-index attribute words from the shape size table */

extern const u16 gUnk_086140EC[];

void ObjShape_InitTables(void)
{
    s32 i;
    for (i = 0; i < 16; i++) {
        s32 width = gUnk_086140EC[i] & 0xFF;
        s32 height = gUnk_086140EC[i] >> 8;
        s32 widthTiles = width >> 3;
        s32 heightTiles = height >> 3;
        gSpriteWidthPixels[i] = width;
        gSpriteHeightPixels[i] = height;
        gSpriteWidthTiles[i] = widthTiles;
        gSpriteHeightTiles[i] = heightTiles;
        gSpriteTileArea[i] = widthTiles * heightTiles;
        gSpriteShapeAttr[i] = ((i & 3) << 14) | ((i & 12) << 28);
    }
}

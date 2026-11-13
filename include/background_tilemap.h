#ifndef GUARD_BACKGROUND_TILEMAP_H
#define GUARD_BACKGROUND_TILEMAP_H
#include "render.h"
void BgGfx_LoadTilemap(u32 layer, s32 tileX, s32 tileY, s32 screenX, s32 screenY, u16 *map);
void BgGfx_LoadTilemapAtPosition(u32 layer, s32 x, s32 y);
void BgGfx_LoadTilemapSimple(u32 layer, s32 tileX, s32 tileY, s32 screenX, s32 screenY);
#endif

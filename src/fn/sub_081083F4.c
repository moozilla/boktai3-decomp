#include "global.h"
#include "ram.h"
/* Provisional names and field interpretations; confidence low. */
#define gTilePool gTileTriggerManager
struct Tile {
    u8 pad0[0x2C];
    u32 resource;
    u8 pad30[0xC0 - 0x30];
    u16 flagsC0;
};
struct TileManager {
    u8 pad0[0x38];
    struct Tile *tiles[12];
    u32 used;
};
void Heap_Free(void *);
void Sprite_Remove(void *);
u32 TerrainOverlay_QueueRemoveActor(u32 *);
void SceneBody_Unregister(void *);
static inline u8 TilePool_IsUsed(struct TileManager *pool, s32 i)
{
    if (pool->used & (1 << i))
        return 1;
    return 0;
}
u32 TilePool_Destroy(struct TileManager *pool)
{
    s32 i;
    struct Tile *tile;
    for (i = 0; i < 12; i++) {
        if (TilePool_IsUsed(pool, i)) {
            tile = pool->tiles[i];
            Sprite_Remove(tile);
            if (tile->flagsC0 & 8) {
                TerrainOverlay_QueueRemoveActor(&tile->resource);
                SceneBody_Unregister(&tile->resource);
            }
            Heap_Free(tile);
        }
    }
    return (u32)(gTileTriggerManager = 0);
}

#include "global.h"
#include "ram.h"
struct Tile {
    u8 pad0[0x8C];
    u32 key;
};
struct TileManager {
    u8 pad0[0x38];
    struct Tile *tiles[12];
    u32 mask;
};
static inline u8 Tst(u32 *m, s32 i)
{
    if (*m & (1 << i))
        return TRUE;
    return FALSE;
}
/* The slot whose key (+0x8C, a name) matches, among the twelve used slots
   of the pool at 0x020001B8; NULL if none. */
struct Tile *Pool1B8_FindByName(u32 key)
{
    struct TileManager *manager = gTileTriggerManager;
    struct Tile **tiles = manager->tiles;
    s32 i;
    if (!manager->mask)
        return 0;
    for (i = 0; i < 12; i++) {
        if (Tst(&manager->mask, i) && tiles[i]->key == key)
            return tiles[i];
    }
    return 0;
}

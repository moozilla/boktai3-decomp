#include "global.h"
#ifdef NONMATCHING
// fuzzy: 97.6%
// Open: the original copies the mask (adds r4, r0) after materializing the
// loop constants.
struct Tile {
    u8 pad0[0x8C];
    u32 key;
};
struct TileManager {
    u8 pad0[0x38];
    struct Tile *tiles[12];
    u32 mask;
};
extern struct TileManager *gUnk_020001B8;
static inline u8 Tst(u32 m, s32 i)
{
    if (m & (1 << i))
        return TRUE;
    return FALSE;
}
struct Tile *Pool1B8_FindByName(u32 key)
{
    struct TileManager *manager = gUnk_020001B8;
    struct Tile **tiles = manager->tiles;
    u32 mask;
    s32 i;
    if (!manager->mask)
        return 0;
    for (i = 0, mask = manager->mask; i < 12; i++) {
        if (Tst(mask, i) && tiles[i]->key == key)
            return tiles[i];
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", Pool1B8_FindByName);
#endif

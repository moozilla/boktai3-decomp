#include "global.h"
struct Map {
    u32 unk0;
    u8 *cells;
    u8 pad[0x1c];
    u16 rows[1];
};
extern struct Map *gUnk_030052F4;
extern u32 gUnk_030052F8;
extern u32 gUnk_030052FC;
u16 sub_0821BDD8(u16 *pos)
{
    struct Map *map = gUnk_030052F4;
    u8 *cells;
    s32 x, z, index;
    if (map->cells == NULL)
        return 0;
    cells = map->cells;
    x = (s16)pos[0] >> 8;
    z = (s16)pos[2] >> 8;
    if (x < 0 || z < 0 || x >= gUnk_030052F8 || z >= gUnk_030052FC)
        index = 0;
    else
        index = map->rows[z] + x;
    return *(u16 *)(cells + index * 4 + 14);
}
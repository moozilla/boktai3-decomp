#include "global.h"
#include "engine.h"

struct Vec081936F8 {
    s16 x, y, z, w;
};
extern const struct Vec081936F8 gUnk_0824F478[];
void TerrainModifiers_RegisterCell(void *, u32, u32, s32, u32, u32);

static inline u32 CellIndex(s32 x, s32 z)
{
    if (x < 0 || z < 0 || x >= gUnk_030052F8 || z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}

void Nidhoggr_RegisterTerrainCells(u8 *t)
{
    if (!t[0x71bd]) {
        TerrainModifiers_RegisterCell(t + 0x7100,
                                      CellIndex(gUnk_0824F478[0].x >> 8, gUnk_0824F478[0].z >> 8),
                                      0, gUnk_0824F478[0].y >> 8, 0xff, 0x1000);
        TerrainModifiers_RegisterCell(t + 0x7110,
                                      CellIndex(gUnk_0824F478[1].x >> 8, gUnk_0824F478[1].z >> 8),
                                      0, gUnk_0824F478[1].y >> 8, 0xff, 0x1000);
        t[0x71bd] = 1;
    }
}

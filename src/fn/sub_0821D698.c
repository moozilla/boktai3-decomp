#include "global.h"
#include "engine_globals.h"
struct N {
    u16 f0;
    u16 f2;
    u8 f4;
    u8 f5;
    u16 f6;
    struct N *f8;
    struct N *fc;
};
struct A {
    u8 p[0x18];
    struct N *f18;
};
void TerrainModifiers_InitCell(struct N *, u32, u32, u32, u32, u32);
u32 TerrainModifiers_RegisterCell(struct N *n, u32 a, u32 b, u32 c, u32 d, u32 e)
{
    TerrainModifiers_InitCell(n, a, b, c, d, e);
    n->f8 = 0;
    n->fc = ((struct A *)gTerrainMap)->f18;
    if (n->fc)
        n->fc->f8 = n;
    ((struct A *)gTerrainMap)->f18 = n;
    return 0;
}

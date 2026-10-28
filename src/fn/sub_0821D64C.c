#include "global.h"
#include "ram.h"
struct N {
    u16 f0;
    u16 f2;
    u8 f4;
    u8 p5[7];
    struct N *fc;
};
struct A {
    u8 p[0x18];
    struct N *f18;
};
struct N *TerrainModifiers_FindCellOverride(u32 k, u32 m)
{
    struct N *best;
    struct N *n = ((struct A *)gTerrainMap)->f18;
    best = 0;
    for (; n; n = n->fc) {
        if ((n->f0 & m) == 0 && n->f2 == k) {
            if (best == 0 || (best->f4 & 0xF) < (n->f4 & 0xF))
                best = n;
        }
    }
    return best;
}

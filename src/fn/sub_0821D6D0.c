#include "global.h"
struct N {
    u8 p[8];
    struct N *f8;
    struct N *fc;
};
struct A {
    u8 p[0x18];
    struct N *f18;
};
extern struct A *gTerrainMap;
void TerrainModifiers_UnregisterCell(struct N *n)
{
    struct N *prev = n->f8;
    struct N *next = n->fc;
    if (prev)
        prev->fc = next;
    else
        gTerrainMap->f18 = next;
    if (next)
        next->f8 = prev;
}

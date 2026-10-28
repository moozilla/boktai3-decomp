#include "global.h"
struct H {
    u16 n;
};
struct A {
    u8 p[0x10];
    struct H *f10;
};
extern struct A *gTerrainMap;
u8 *Path_GetDefinition(u32 i)
{
    struct H *h = gTerrainMap->f10;
    return i < h->n ? (u8 *)h + 4 + (i << 2) : 0;
}

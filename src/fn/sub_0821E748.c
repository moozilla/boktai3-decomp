#include "global.h"
#include "engine_globals.h"
struct H {
    u16 n;
};
struct A {
    u8 p[0x10];
    struct H *f10;
};
u8 *Path_GetDefinition(u32 i)
{
    struct H *h = ((struct A *)gTerrainMap)->f10;
    return i < h->n ? (u8 *)h + 4 + (i << 2) : 0;
}

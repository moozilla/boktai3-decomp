#include "global.h"
#include "engine.h"
struct A {
    u8 p[4];
    u16 *f4;
    u8 q[0x14];
    s16 f1c;
    u16 f1e;
    u16 f20;
    u16 f22;
};
void Terrain_SetNeighborOffsets(void)
{
    struct A *a = gTerrainMap;
    u16 *s = a->f4;
    a->f1c = -s[2];
    a->f1e = 1;
    a->f20 = s[2];
    a->f22 = 0xFFFF;
}

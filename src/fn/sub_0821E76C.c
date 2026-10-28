#include "global.h"
struct A {
    u8 p[0x10];
    u8 *f10;
};
extern struct A *gTerrainMap;
u8 *Path_GetPointData(u16 *d)
{
    u8 *b = gTerrainMap->f10;
    return b + d[1];
}

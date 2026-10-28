#include "global.h"
#include "ram.h"
struct A {
    u8 p[0x10];
    u8 *f10;
};
u8 *Path_GetPointData(u16 *d)
{
    u8 *b = ((struct A *)gTerrainMap)->f10;
    return b + d[1];
}

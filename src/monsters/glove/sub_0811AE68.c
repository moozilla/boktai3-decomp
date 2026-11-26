#include "global.h"
struct H {
    u32 lo : 16;
    u32 hi : 16;
};
struct Z {
    struct H a, b;
};
u32 Terrain_GetCellClearance(struct Z *);
u32 Terrain_GetHeightAtFixedPosition(u32 *p)
{
    struct Z z;
    z.a.lo = (p[0] << 4) >> 16;
    z.b.lo = (p[2] << 4) >> 16;
    return Terrain_GetCellClearance(&z);
}

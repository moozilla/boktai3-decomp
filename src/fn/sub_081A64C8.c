#include "global.h"
struct Vec3s {
    s16 x, y, z;
};
extern u16 gUnk_03005418[];
void BgGfx_LoadTilemapAtPosition(u32, s32, s32);
static inline s32 Shift5(s32 x)
{
    s32 r;
    if (x >= 0)
        r = x >> 5;
    else
        r = -((-x) >> 5);
    return r;
}
static inline void Project(struct Vec3s *out, struct Vec3s *v)
{
    s32 s, h;
    s32 x, z;
    x = v->x;
    z = v->z;
    out->x = Shift5((x - z) * 3);
    s = Shift5((x + z) * 3);
    h = Shift5(v->y * 3);
    out->y = s - h;
    out->z = s + h;
}
void ScreenPalette_ProjectPosition(u32 layer, struct Vec3s *pos, s32 xOff, s32 yOff)
{
    struct Vec3s d;
    Project(&d, pos);
    d.x = gUnk_03005418[0] - d.x;
    d.y = gUnk_03005418[1] - d.y;
    d.z = d.z - gUnk_03005418[2];
    BgGfx_LoadTilemapAtPosition(layer, d.x + xOff, d.y + yOff);
}

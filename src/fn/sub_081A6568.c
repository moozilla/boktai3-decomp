#include "global.h"
#include "render.h"
struct Vec3s {
    s16 x, y, z;
};
struct PositionedObject {
    u8 pad0[8];
    struct Vec3s pos;
};
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
void ScreenPalette_ProjectPositionAt88(struct PositionedObject *obj)
{
    struct Vec3s d;
    Project(&d, &obj->pos);
    d.x = gCameraOrigin[0] - d.x;
    d.y = gCameraOrigin[1] - d.y;
    d.z = d.z - gCameraOrigin[2];
    BgGfx_LoadTilemapAtPosition(2, d.x + 0x88, d.y + 0x6c);
}

#include "global.h"

struct Vec081935B8 {
    s16 x, y, z;
};
extern u16 gUnk_03005418[];
void sub_08216520(u32, s32, s32);

static inline s32 Shift5(s32 x)
{
    s32 r;
    if (x >= 0)
        r = x >> 5;
    else
        r = -((-x) >> 5);
    return r;
}

static inline void Project(struct Vec081935B8 *out, struct Vec081935B8 *v)
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

void sub_081935B8(u8 *t)
{
    struct Vec081935B8 d;
    Project(&d, (struct Vec081935B8 *)(t + 0x5404));
    d.x = gUnk_03005418[0] - d.x;
    d.y = gUnk_03005418[1] + 10 - d.y;
    d.z = d.z - gUnk_03005418[2];
    {
        s16 tmp = d.x;
        *(s16 *)(t + 0x5416) = tmp;
    }
    *(s16 *)(t + 0x5418) = d.y;
    sub_08216520(3, *(s16 *)(t + 0x5416), *(s16 *)(t + 0x5418));
    sub_08216520(1, *(s16 *)(t + 0x5416), *(s16 *)(t + 0x5418));
}

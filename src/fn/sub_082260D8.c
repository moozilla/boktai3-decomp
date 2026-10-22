#include "global.h"
struct Camera {
    u8 pad[0x7e];
    u16 shake;
    u16 offsetX;
    u16 offsetZ;
};
struct Vec {
    s16 x, y, z;
};
extern struct Camera *gUnk_03005420;
extern struct Vec gUnk_030042A0;
extern struct Vec gUnk_03005418;
static inline s32 scale(s32 input)
{
    s32 value = input * 3;
    if (value >= 0)
        value >>= 5;
    else
        value = -((-value) >> 5);
    return value;
}
static inline s32 scaleHeight(s32 input)
{
    s32 value = input * 3;
    if (value >= 0)
        return value >> 5;
    return -((-value) >> 5);
}
static inline void project(struct Vec *out, struct Vec *in)
{
    s32 x;
    s32 z, h, y;
    x = in->x;
    z = in->z;
    out->x = scale(x - z);
    {
        s32 sum = x + z;
        h = scale(sum);
    }
    y = scaleHeight(in->y);
    out->y = h - y;
    out->z = h + y;
}
void sub_082260D8(void)
{
    if (gUnk_03005420) {
        gUnk_030042A0.x -= gUnk_03005420->offsetX;
        gUnk_030042A0.z -= gUnk_03005420->offsetZ;
        project(&gUnk_03005418, &gUnk_030042A0);
        gUnk_03005420->shake = 0;
        gUnk_03005420->offsetX = 0;
        gUnk_03005420->offsetZ = 0;
    }
}

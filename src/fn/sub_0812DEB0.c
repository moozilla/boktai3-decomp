#include "global.h"
extern const s16 gUnk_086149C4[];
int Math_Atan2(int, int);
struct Vec {
    s16 x, y, z;
};

bool32 sub_0812DEB0(struct Vec *from, struct Vec *to, struct Vec *vel, u8 *angleOut, u32 rangeX,
                    u32 rangeZ, s32 speed)
{
    u16 ax = to->x - from->x, az = to->z - from->z;
    u8 angle = Math_Atan2((s16)ax, (s16)az);
    s32 d;
    vel->x = (gUnk_086149C4[(angle + 0x40) & 0xff] * speed) >> 16;
    vel->z = (gUnk_086149C4[angle] * speed) >> 16;
    *angleOut = angle;
    d = from->x - to->x;
    if (d < 0)
        d = -d;
    if (d <= rangeX) {
        d = from->z - to->z;
        if (d < 0)
            d = -d;
        if (d <= rangeZ)
            return TRUE;
    }
    return FALSE;
}

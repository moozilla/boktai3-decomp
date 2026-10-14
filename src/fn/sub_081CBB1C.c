#include "global.h"
#ifdef NONMATCHING
// fuzzy: 47.1%
// register allocation: the original keeps pos/target/heading/velocity in r6/r8/sb/sl
struct Vec081CBB1C {
    s16 x, y, z, pad;
};
struct Pair081CBB1C {
    u32 a, b;
};
extern const s16 gUnk_086149C4[];
u8 Math_Atan2(s32, s32);
static inline s32 shift12(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}
/* Walk toward the target at +0x54: snap and finish when within the squared
 * radius at +0x20C, else step along the heading and turn toward the target. */
void sub_081CBB1C(u8 *a, u8 *b)
{
    struct Vec081CBB1C *pos, *target, *vel;
    u8 *heading;
    s32 dx, dz;
    if (b[9]) {
        b[9] = 0;
        b[7] = 0;
    }
    pos = (struct Vec081CBB1C *)(b + 0x68);
    target = (struct Vec081CBB1C *)(b + 0x54);
    dx = target->x - pos->x;
    dz = target->z - pos->z;
    if ((u32)(dx * dx + dz * dz) <= *(u32 *)(b + 0x20c)) {
        *(struct Pair081CBB1C *)pos = *(struct Pair081CBB1C *)target;
        b[7] = 1;
        return;
    }
    heading = b + 0x65;
    vel = (struct Vec081CBB1C *)(b + 0x70);
    {
        u8 angle = *heading;
        s32 speed = *(s32 *)(b + 0x208);
        vel->x = shift12(speed * gUnk_086149C4[(angle + 64) & 255]);
        vel->y = 0;
        vel->z = shift12(speed * gUnk_086149C4[angle]);
    }
    b[0x1b] = Math_Atan2(target->x - pos->x, target->z - pos->z);
    *heading = b[0x1b];
    b[0x1a] = ((*heading + 0x70) >> 5) & 7;
    b[0x19] = ((*heading + 0x60) >> 6) & 3;
    pos->x += vel->x;
    pos->z += vel->z;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081CBB1C);
#endif

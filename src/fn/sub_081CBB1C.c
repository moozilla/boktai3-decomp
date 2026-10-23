#include "global.h"
// MATCHING_CONCESSION: register-pinning
#ifdef NONMATCHING
// fuzzy: 70.8%
// fix.py best version (reprinted C)
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
    u32 k1;
    if (x >= 0)
        return x >> (k1 = 12);
    return -((-x) >> k1);
}

static inline void Polar(struct Vec081CBB1C *v, u32 angle, s32 speed)
{
    v->x = shift12(speed * gUnk_086149C4[(angle + 64) & 255]);
    v->y = 0;
    v->z = shift12(speed * gUnk_086149C4[angle]);
}
/* Walk toward the target at +0x54: snap and finish when within the squared
 * radius at +0x20C, else step along the heading and turn toward the target. */
void sub_081CBB1C(u8 *a, u8 *input)
{
    register u8 *b asm("r5") = input;
    struct Vec081CBB1C *pos;
    struct Vec081CBB1C *target;
    struct Vec081CBB1C *vel;
    register struct Vec081CBB1C *savedPos asm("r6");
    register struct Vec081CBB1C *savedTarget asm("r8");
    u32 distance;
    u32 limit;
    u8 *heading;
    s32 dx;
    s32 dz;
    typeof(*heading) t1;
    if (b[9]) {
        b[9] = 0;
        b[7] = 0;
    }
    pos = (struct Vec081CBB1C *)(b + 0x68);
    target = (struct Vec081CBB1C *)(b + 0x54);
    dx = target->x - pos->x;
    dz = target->z - pos->z;
    distance = (((u32)dx) * dx) + (((u32)dz) * dz);
    limit = *((u32 *)(b + 0x20C));
    savedPos = pos;
    savedTarget = target;
    if (distance <= limit) {
        *((struct Pair081CBB1C *)pos) = *((struct Pair081CBB1C *)target);
        b[7] = 1;
        return;
    }
    heading = b + 0x65;
    vel = (struct Vec081CBB1C *)(b + 0x70);
    t1 = *heading;
    Polar(vel, t1, *((s32 *)(b + 0x208)));
    b[0x1b] = Math_Atan2(target->x - savedPos->x, target->z - savedPos->z);
    *heading = b[0x1b];
    b[0x1a] = ((b[0x1b] + 0x70) >> 5) & 7;
    b[0x19] = ((b[0x1b] + 0x60) >> 6) & 3;
    savedPos->x += vel->x;
    savedPos->z += vel->z;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081CBB1C);
#endif

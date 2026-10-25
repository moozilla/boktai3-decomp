#include "global.h"

struct F080A9BD8Vec {
    s16 x, y, z, pad;
};
struct F080A9BD8Seg {
    u8 pad0[0x1c];
    struct F080A9BD8Vec position;
    u8 pad24[0x128 - 0x24];
};
struct F080A9BD8Group {
    u8 pad0[0x3d4];
    struct F080A9BD8Seg segments[4];
};
struct F080A9BD8Actor {
    u32 pad0;
    void *drawable;
    u8 pad8[0x3c8];
    struct F080A9BD8Group *group;
};
static inline s32 F080A9BD8_Q10(s32 x)
{
    if (x < 0)
        return -((-x) >> 10);
    return x >> 10;
}
static inline void F080A9BD8_Diff(struct F080A9BD8Vec *d, struct F080A9BD8Vec *a,
                                  struct F080A9BD8Vec *b)
{
    d->x = a->x - b->x;
    d->y = a->y - b->y;
    d->z = a->z - b->z;
}

#ifdef NONMATCHING
// fuzzy: 25.8%
// scope shape 172 (differences once low registers are set aside)
// confidence=low: blends a run of segment positions from head toward target along a cubic curve.
void sub_080A9BD8(void *arg0, void *arg1, void *arg2, s32 arg3, s32 arg4)
{
    struct F080A9BD8Group *g = ((struct F080A9BD8Actor *)arg0)->group;
    struct F080A9BD8Vec head = *(struct F080A9BD8Vec *)arg1;
    struct F080A9BD8Vec target = *(struct F080A9BD8Vec *)arg2;
    s32 step = Div(0x400, arg4 - arg3 + 1);
    s32 n;
    s32 k = 0;
    s32 zero = 0;

    if (arg3 < arg4) {
        struct F080A9BD8Seg *seg = &g->segments[arg3];
        n = arg4 - arg3;
        do {
            s32 t = step * (k + 1);
            struct F080A9BD8Vec tan;
            struct F080A9BD8Vec delta;
            struct F080A9BD8Vec out;
            struct F080A9BD8Vec *o = &out;
            s32 u;
            s32 v;
            s32 h;
            s32 w;
            s32 zz;

            F080A9BD8_Diff(&delta, &target, &head);
            ZeroFill8(&tan);
            u = F080A9BD8_Q10(t * t);
            v = F080A9BD8_Q10(u * t);
            h = 2 * v - 3 * u + 1;
            w = 3 * u - 2 * v;
            zz = zero * (v - 2 * u + t);
            o->x = head.x + F080A9BD8_Q10(tan.x * h * zero + delta.x * w + (v - u) + zz);
            o->y = head.y + F080A9BD8_Q10(tan.y * h * zero + delta.y * w + (v - u) + zz);
            o->z = head.z + F080A9BD8_Q10(tan.z * h * zero + delta.z * w + (v - u) + zz);
            seg->position = out;
            seg++;
            k++;
        } while (--n != 0);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080A9BD8);
#endif

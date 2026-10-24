#include "global.h"
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: register allocation, stack lifetimes and instruction ordering.
struct Pair {
    u32 a, b;
};
struct Collider {
    u8 pad[6];
    u16 flags;
    u8 pad8[12];
    struct Pair bounds;
};
struct Actor {
    u8 pad[8];
    u8 resource[0x20];
    u8 anim[8];
    u32 flags;
    u8 pad34[8];
    u16 frame;
    u8 pad3E[10];
    s16 x, y, z;
    u8 pad4E[0xf4 - 0x4e];
    struct Collider collider;
    u8 pad110[0x148 - 0x110];
    s16 dx, dy, dz;
    u8 pad14E[6];
    u8 entered, timer, done, state;
};
extern const struct Pair gUnk_0824F65C, gUnk_0824F63C, gUnk_0824F664, gUnk_0824F644, gUnk_0824F66C,
    gUnk_0824F64C, gUnk_0824F674, gUnk_0824F654;
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void SceneEffect_UpdateRecoil(struct Actor *a)
{
    s32 state = a->entered;
    u16 k;
    u8 *t;
    typeof(a->timer) *q1 = &a->timer;
    switch (state) {
        case 0:
            Anim_SetKeyframes(a->anim, a->resource, 4, 2, 0);
            a->entered++;
            break;

        case 1:
            t = q1;
            if ((*t) <= 7)
                goto increment;
            else {
                {
                    u8 k1 = 2;
                    a->entered = k1;
                }
                a->timer = 0;
                if (a->flags & 4) {
                    a->dx = -48;
                    a->dy = 0;
                    a->dz = 48;
                } else {
                    a->dx = 48;
                    a->dy = 0;
                    a->dz = -48;
                }
            }
            break;

        case 2:
            a->x += a->dx;
            a->z += a->dz;
            if (a->flags & 4) {
                a->dx += 2;
                a->dz -= 2;
                if (a->dx > 0) {
                    a->dx = 0;
                    a->dz = 0;
                }
            } else {
                a->dx -= 2;
                a->dz += 2;
                if (a->dx < 0) {
                    a->dx = 0;
                    a->dz = 0;
                }
            }
            if (a->done) {
                a->entered++;
                a->timer = 0;
            }
            break;

        default:
            t = q1;
            if ((*t) <= 3) {
            increment:
                (*t)++;

            } else {
                k = 8;
                a->state = k;
                a->entered = 0;
                a->timer = 0;
            }
            break;
    }

    {
        struct Collider *c = &a->collider;
        switch (a->frame) {
            case 5:

            case 7:
                if (a->flags & 4)
                    c->bounds = gUnk_0824F664;
                else
                    c->bounds = gUnk_0824F644;
                break;

            case 6: {
                u32 mask = ~4;
                c->flags = mask & c->flags;
            }
                if (a->flags & 4)
                    c->bounds = gUnk_0824F65C;
                else
                    c->bounds = gUnk_0824F63C;
                break;

            case 15:
                if (a->flags & 4)
                    c->bounds = gUnk_0824F66C;
                else
                    c->bounds = gUnk_0824F64C;
                break;

            case 16:
                if (a->flags & 4)
                    c->bounds = gUnk_0824F674;
                else
                    c->bounds = gUnk_0824F654;
                break;

            default:
                c->flags |= 4;
                break;
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", SceneEffect_UpdateRecoil);
#endif

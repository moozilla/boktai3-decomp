#include "global.h"
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
static inline u8 *SceneEffect_GetRecoilTimer(struct Actor *a)
{
    return &a->timer;
}
void SceneEffect_UpdateRecoil(struct Actor *a)
{
    s32 state = a->entered;
    u16 nextState;
    switch (state) {
        case 0:
            Anim_SetKeyframes(a->anim, a->resource, 4, 2, 0);
            a->entered++;
            break;

        case 1:
            if (a->timer <= 7)
                a->timer++;
            else {
                {
                    u8 nextPhase = 2;
                    a->entered = nextPhase;
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

        default: {
            u8 *timer = SceneEffect_GetRecoilTimer(a);
            if (*timer <= 3) {
                (*timer)++;
            } else {
                nextState = 8;
                a->state = nextState;
                a->entered = 0;
                a->timer = 0;
            }
            break;
        }
    }

    {
        switch (a->frame) {
            case 5:
            case 6: {
                struct Collider *c = &a->collider;
                u32 mask = 4;
                c->flags = ~mask & c->flags;
                if (a->flags & mask)
                    c->bounds = gUnk_0824F65C;
                else
                    c->bounds = gUnk_0824F63C;
                break;
            }
            case 7: {
                if (a->flags & 4) {
                    struct Collider *c = &a->collider;
                    c->bounds = gUnk_0824F664;
                } else {
                    struct Collider *c = &a->collider;
                    c->bounds = gUnk_0824F644;
                }
                break;
            }
            case 15: {
                if (a->flags & 4) {
                    struct Collider *c = &a->collider;
                    c->bounds = gUnk_0824F66C;
                } else {
                    struct Collider *c = &a->collider;
                    c->bounds = gUnk_0824F64C;
                }
                break;
            }
            case 16: {
                if (a->flags & 4) {
                    struct Collider *c = &a->collider;
                    c->bounds = gUnk_0824F674;
                } else {
                    struct Collider *c = &a->collider;
                    c->bounds = gUnk_0824F654;
                }
                break;
            }
            default: {
                struct Collider *c = &a->collider;
                c->flags |= 4;
                break;
            }
        }
    }
}

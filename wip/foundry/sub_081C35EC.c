#include "global.h"

#define SceneActorMotion_Begin sub_081C35EC

struct Vec {
    s16 x, y, z;
};

struct Copy8 {
    s32 w0, w1;
};

extern struct Vec gUnk_03005418;
void sub_081C3B04(void *);

struct TransitionTarget {
    u8 pad00[0x34];
    u32 flags;        /* 0x34 */
    u8 pad38[0x50 - 0x38];
    s16 pos[2];        /* 0x50 */
    u8 pad54[0x98 - 0x54];
    u16 skipFlag;     /* 0x98 */
    u16 duration;     /* 0x9a */
    u8 pad9C[0xd8 - 0x9c];
    s32 progress;     /* 0xd8 */
    s16 dx;           /* 0xdc */
    s16 dy;           /* 0xde */
    s16 dz;           /* 0xe0 */
    u8 padE2[0xf4 - 0xe2];
    void (*tick)(struct TransitionTarget *); /* 0xf4 */
    s32 tickTimer;    /* 0xf8 */
};

static inline s32 Halve(s32 v)
{
    return v >= 0 ? v >> 1 : -(-v >> 1);
}

static inline void SetCallback(void (*fn)(struct TransitionTarget *), void (**slot)(struct TransitionTarget *))
{
    *slot = fn;
}

void sub_081C35EC(struct TransitionTarget *p, void *src, u32 duration)
{
    s32 a, b, v;

    if (p == NULL)
        return;

    if (src != NULL) {
        if (p->skipFlag == 0) {
            struct Vec *out = (struct Vec *)&p->dx;
            a = ((s16 *)src)[0] + gUnk_03005418.x - 0x78;
            b = ((s16 *)src)[1] + gUnk_03005418.y - 0x50;
            v = (a + b) * 32 / 3;
            if (v >= 0)
                out->x = v >> 1;
            else
                out->x = -(-v >> 1);
            out->y = 0;
            v = (b - a) * 32 / 3;
            out->z = Halve(v);
        } else {
            struct Copy8 *out = (struct Copy8 *)&p->dx;
            *out = *(struct Copy8 *)src;
            goto common;
        }
    } else {
        struct Vec *out = (struct Vec *)&p->dx;
        s16 *coords = p->pos;
        a = coords[0] + gUnk_03005418.x - 0x78;
        b = coords[1] + gUnk_03005418.y - 0x50;
        v = (a + b) * 32 / 3;
        if (v >= 0)
            out->x = v >> 1;
        else
            out->x = -(-v >> 1);
        out->y = 0;
        v = (b - a) * 32 / 3;
        out->z = Halve(v);
    }

common:
    p->duration = duration;
    p->flags &= -2;
    SetCallback(sub_081C3B04, &p->tick);
    p->tickTimer = 0;
    p->progress = 0;
}

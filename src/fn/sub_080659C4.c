#include "global.h"

struct V { u32 a, b; };
struct Q {
    u32 flags;
    u8 pad4[0xc];
    u16 w10;
    u8 pad12[0xa];
    union {
        struct V v;
        struct { u16 x, y, z; } h;
    } pos;
};
struct O {
    struct V *src;
    u8 pad4[0x18];
    u16 x, y, z;
    u8 pad22[0xc];
    s16 dy;
    u8 f30;
    u8 pad31[5];
    u8 f36;
    u8 pad37[2];
    u8 ang;
    u8 pad3a[0x26];
    struct Q q;
};
extern const s16 gUnk_086149C4[];
extern u8 *gUnk_02000124;
void sub_08065904(struct O *);
u8 sub_0806466C(u8 *, struct O *);

static inline u32 tst(u8 *a, u32 m)
{
    return *a & m;
}

static inline u8 test(u8 *p, u32 m)
{
    if (tst(p, m))
        return TRUE;
    return FALSE;
}

void sub_080659C4(struct O *o)
{
    u8 *e = gUnk_02000124;
    if (e != NULL) {
        u8 *e2 = e;
        struct Q *q = &o->q;
        q->flags &= -2;
        sub_08065904(o);
        q->w10 = o->f30;
        if (o->src != NULL) {
            s32 dy;
            u32 one;
            u32 ca, cb;
            struct V *v = o->src;
            cb = v->b;
            ca = v->a;
            q->pos.v.a = ca;
            q->pos.v.b = cb;
            dy = o->dy;
            one = 1;
            if (!test(&o->f36, one)) {
                s32 t = gUnk_086149C4[o->ang];
                dy += t >> 8;
            }
            q->pos.h.x = q->pos.h.x + o->x;
            q->pos.h.y = o->y + dy + q->pos.h.y;
            q->pos.h.z = q->pos.h.z + o->z;
        }
        if (!sub_0806466C(e2, o))
            o->ang += 4;
    }
}

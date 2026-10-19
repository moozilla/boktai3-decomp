#include "global.h"
#include "engine.h"

struct Pair { u32 a, b; };
struct HP { s16 a, b; };
struct S {
    u8 pad0[0x54];
    s16 px, py, pz;
    u8 pad5a[0x9c - 0x5a];
    u32 fl;
    u8 pada0[2];
    u8 state;
    u8 pada3;
    u32 cnt;
    u8 padA8[0xaf - 0xa8];
    u8 fAF;
    u8 padB0;
    u8 fB1;
    u8 fB2;
    u8 padB3[0xcc - 0xb3];
    u8 fCC;
    u8 padCD[0x104 - 0xcd];
    void *f104;
    u8 pad108[0x110 - 0x108];
    void *f110;
    u8 pad114[0x18c - 0x114];
    u8 a[0x3fc - 0x18c];
    u8 b3fc[0x444 - 0x3fc];
    u8 anim[0x500 - 0x444];
    u16 f500;
    u8 pad502[0x534 - 0x502];
    u8 g534[0x55d - 0x534];
    u8 f55D;
    u8 pad55E[0x56c - 0x55e];
    u8 *f56C;
    u8 pad570[0x57c - 0x570];
    void *f57C;
    u8 pad580[0x5b0 - 0x580];
    u16 f5B0;
    u16 pad5B2;
    u16 f5B4;
    u8 pad5B6[0x5c7 - 0x5b6];
    u8 f5C7;
};
extern struct Pair gUnk_0824F788;
extern void sub_081B6244(void *, u32, u32);
extern s32 sub_081B64D4(void *);
extern s32 sub_081B658C(void *);
extern void sub_081B70A8(void *, void *, void *, s32, s32, s32);
extern void sub_081B71D4(void *, void *, void *, void *, s32, s32, s32);
extern void sub_081B9334(void *, void *, void *, u32, u32);
extern s32 sub_08249350(s32, s32);

static inline s32 rnd(void)
{
    u32 off;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    off = gUnk_03005308 * 2;
    return *(const u16 *)(off + (u32)gUnk_0203B400);
}

static inline s32 m8(s32 v)
{
    return v - ((v >> 3) << 3);
}

static inline s32 m32(s32 v)
{
    return v - ((v >> 5) << 5);
}

static inline s32 half(s32 n)
{
    return n >= 0 ? n >> 1 : -((-n) >> 1);
}

void sub_081AA788(struct S *p)
{
    struct S *q = p;
    u8 *a = p->a;
    u32 r;

    {
        u32 m = 0x40;
        p->fl |= m;
    }
    sub_081A6E9C(p, 0);
    r = (u8)sub_0824923C(p, p->f104);
    if (r != 0)
        return;
    if (p->fB1 != 0) {
        u32 z = 0;
        p->fB1 = r;
        p->f5B4 = r;
        sub_081B6244(p->f56C, 3, 0x13);
        p->state = z;
        p->cnt = r;
    }
    switch (p->state) {
    case 0:
        if (sub_081B64D4(q->f56C)) {
            sub_081B6244(q->f56C, 3, 2);
            p->state = 1;
            p->cnt = 0;
        }
        break;
    case 1: {
        u32 z = p->cnt;
        u8 *w;
        if (z == 0) {
            SceneActors_StartUnmirroredAnimation(p, a, a + 0x60, 8, 2, z);
            p->cnt++;
            w = q->b3fc;
            Anim_SetSequence(q->anim, *(void **)(w + 0x58), 0, 0, z);
            q->f500 = 1;
            *(u32 *)(w + 0x5c) = z;
        }
        if (p->fB2 != 0) {
            p->state = 2;
            p->cnt = 0;
        }
        break;
    }
    case 2: {
        u32 n, m;
        u32 k = 0x80000;
        p->fl |= k;
        if (p->cnt <= 3)
            q->f5B0 = 8;
        n = p->cnt + 1;
        p->cnt = n;
        m = 0x10;
        if (q->f5C7 > 1)
            m = 8;
        if (n & m) {
            p->state = 3;
            p->cnt = 0;
        }
        break;
    }
    case 3: {
        u32 z;
        u32 k = 0x180000;
        p->fl |= k;
        z = p->cnt;
        if (z == 0) {
            s16 *pos = &p->px;
            s16 v[3];
            u32 t32;
            s16 *tp;
            s32 vx, vy, vz;
            s32 cc0;
            u8 *d;
            s32 x, y;
            struct Pair tbl;
            s32 sl;
            u8 *m;
            struct S *s;
            vx = p->px;
            vy = p->py;
            vz = p->pz;
            v[0] = vx - 0x100;
            v[1] = vy;
            v[2] = vz - 0x100;
            sub_081B9334(q->g534, pos, v, 0, 1);
            SceneActors_StartUnmirroredAnimation(p, a, a + 0x60, 9, 2, z);
            d = a + 0x128;
            t32 = z;
            tp = (s16 *)&t32;
            x = tp[0];
            y = tp[1];
            *(s16 *)d = half(__divsi3((x + y) << 5, 3) + 0x40);
            *(s16 *)(d + 2) = 0x40;
            *(s16 *)(d + 4) = half(__divsi3((y - x) << 5, 3) + 0x40);
            s = p;
            tbl = gUnk_0824F788;
            if (p->f5C7 > 1) {
                sl = m8(rnd() >> 3) + 0x14;
            } else {
                sl = m8(rnd() >> 3) + 0xc;
            }
            if (!sub_081B658C(s->f56C))
                sl += 8;
            m = s->f56C;
            {
                u32 e = ((u32 *)&tbl)[s->f5B4 & 1];
                if (*(s16 *)(m + e * 540 + 0x20c) > 0) {
                    cc0 = p->fCC;
                    sub_081B71D4(s->f57C, pos, p->f110, m + e * 540 + 0xb4, cc0 + (m32(rnd() >> 3) - 16), sl, sub_08249350(rnd() >> 3, 6));
                } else {
                    e = ((u32 *)&tbl)[((s16)s->f5B4 + 1) & 1];
                    if (*(s16 *)(m + e * 540 + 0x20c) > 0) {
                        cc0 = p->fCC;
                    sub_081B71D4(s->f57C, pos, p->f110, m + e * 540 + 0xb4, cc0 + (m32(rnd() >> 3) - 16), sl, sub_08249350(rnd() >> 3, 6));
                    } else {
                        cc0 = p->fCC;
                        sub_081B70A8(s->f57C, pos, p->f110, cc0 + (m32(rnd() >> 3) - 16), sl, sub_08249350(rnd() >> 3, 6));
                    }
                }
            }
            Sound_PlaySong(0x583);
            p->cnt++;
        }
        {
            u32 cc = p->fCC;
            u8 *e = a + 0xa8;
            u32 f = 0x17;
            if (cc > 0x10) {
                f = 0x18;
                if (cc > 0x2f) {
                    f = 0x19;
                    if (cc > 0x40) {
                        f = 0x16;
                        if (cc <= 0x9f)
                            f = 0x1a;
                    }
                }
            }
            sub_08214588(e, e + 0x2c);
            *(u16 *)(e + 0x10) = f;
        }
        sub_081B95E4(q->g534);
        p->cnt++;
        {
            u32 fl = 0;
            if (q->f55D == 2)
                fl = 1;
            if (fl) {
                q->f5B4++;
                p->state = 4;
                p->cnt = 0;
            }
        }
        break;
    }
    case 4: {
        u32 fl;
        u32 k = 0x180000;
        p->fl |= k;
        if (p->cnt == 0) {
            s16 *pos = &p->px;
            s16 v[3];
            s32 vx = p->px;
            s32 vy = p->py;
            s32 vz = p->pz;
            v[0] = vx + 0x100;
            v[1] = vy;
            v[2] = vz + 0x100;
            if (q->f5C7 > 1)
                sub_081B9334(q->g534, pos, v, 0, 2);
            else
                sub_081B9334(q->g534, pos, v, 0, 4);
            p->cnt++;
        }
        sub_081B95E4(q->g534);
        fl = 0;
        if (q->f55D == 2)
            fl = 1;
        if (fl) {
            if ((s16)q->f5B4 > 3) {
                p->state = 5;
                p->cnt = 0;
            } else {
                p->state = 2;
                p->cnt = 0;
            }
        }
        break;
    }
    case 5: {
        u32 z = p->cnt;
        u8 *w;
        if (z == 0) {
            SceneActors_StartUnmirroredAnimation(p, a, a + 0x60, 8, 2, 2);
            p->cnt++;
            w = q->b3fc;
            Anim_SetSequence(q->anim, *(void **)(w + 0x58), 0, 0, 4);
            q->f500 = 1;
            *(u32 *)(w + 0x5c) = z;
        }
        if (p->fB2 != 0) {
            p->state = 6;
            p->cnt = 0;
        }
        break;
    }
    case 6:
        p->fAF = 1;
        break;
    }
}

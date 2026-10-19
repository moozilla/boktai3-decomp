#include "global.h"

struct Q {
    u8 pad[0x58];
    u32 f454;
    u32 f458;
};

struct P {
    u8 pad0[0x54];
    s16 x, y, z;
    u8 pad1[0x9c - 0x5a];
    u32 flags;
    u8 pad2[0xa2 - 0xa0];
    u8 state;
    u8 pad3;
    u32 cnt;
    u8 pad4[0xaf - 0xa8];
    u8 f_af;
    u8 pad5;
    u8 f_b1;
    u8 f_b2;
    u8 pad6[0xcc - 0xb3];
    u8 cc;
    u8 pad7[0x104 - 0xcd];
    void *f104;
    u8 pad8[0x110 - 0x108];
    void *f110;
    u8 pad9[0x18c - 0x114];
    u8 sub[0x3fc - 0x18c];
    u8 pad10[0x444 - 0x3fc];
    u8 o444[0x500 - 0x444];
    u16 f500;
    u8 pad12[0x534 - 0x502];
    u8 o534[0x55d - 0x534];
    u8 f55d;
    u8 pad13[0x56c - 0x55e];
    u8 *f56c;
    u8 pad14[0x57c - 0x570];
    void *f57c;
    u8 pad15[0x5b0 - 0x580];
    u16 f5b0;
    s16 f5b4;
    u8 pad16[0x5c7 - 0x5b6];
    u8 f5c7;
};

struct E {
    u8 pad[0xb4];
    u8 b4[0x20c - 0xb4];
    s16 v;
    u8 pad2[540 - 0x20e];
};

extern const u32 gUnk_0824F788[2];
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];

void sub_081A6E9C(struct P *p, u16 v);
void sub_081B95E4(void *);
void sub_08214588(void *, void *);
s32 sub_0824923C(void *arg, void *function);
s32 sub_08249350(s32, s32);
void sub_08021890(void *, void *, void *, int, int, int);
void sub_081B6244(void *, int, int);
int sub_081B64D4(void *);
int sub_081B658C(void *);
void sub_081B70A8(void *, void *, void *, int, int, int);
void sub_081B71D4(void *, void *, void *, void *, int, int, int);
void sub_081B9334(void *, void *, void *, int, int);
void sub_08220C8C(void *, u32, int, int, int);
void sub_0822B2F8(int);
s32 sub_08249274(s32, s32);

#define GO(n) do { u8 s_ = n; u32 z_ = 0; *st = s_; *cp = z_; } while (0)
#define GOP(n) do { u8 s_ = n; u32 z_ = 0; *st = s_; p->cnt = z_; } while (0)
#define RND() (gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF, gUnk_0203B400[gUnk_03005308])

void sub_081AA788(struct P *p)
{
    struct P *q = p;
    u8 *sub = p->sub;
    u8 r, b1v;
    u8 *st;

    u32 m = 0x40;
    p->flags |= m;
    sub_081A6E9C(p, 0);
    r = sub_0824923C(p, p->f104);
    if (r != 0)
        return;
    b1v = p->f_b1;
    st = &p->state;
    if (b1v != 0) {
        p->f_b1 = r;
        p->f5b4 = r;
        sub_081B6244(q->f56c, 3, 0x13);
        *st = 0;
        p->cnt = r;
    }
    switch (*st) {
    case 0:
        if (sub_081B64D4(q->f56c) != 0) {
            sub_081B6244(q->f56c, 3, 2);
            GOP(1);
        }
        break;
    case 1: {
        u32 *cp = &p->cnt;
        u32 cnt = *cp;
        if (cnt == 0) {
            struct Q *qq;
            sub_08021890(p, sub, sub + 0x60, 8, 2, cnt);
            (*cp)++;
            qq = (struct Q *)((u8 *)q + 0x3fc);
            sub_08220C8C(q->o444, qq->f454, 0, 0, cnt);
            q->f500 = 1;
            qq->f458 = cnt;
        }
        if (p->f_b2 != 0) {
            GO(2);
        }
        break;
    }
    case 2: {
        u32 *cp;
        u32 n, m, c;
        u32 mk = 0x80000;
        p->flags |= mk;
        cp = &p->cnt;
        if (*cp <= 3)
            q->f5b0 = 8;
        n = *cp + 1;
        *cp = n;
        c = q->f5c7;
        m = 0x10;
        if (c > 1)
            m = 8;
        if (n & m)
            GO(3);
        break;
    }
    case 3: {
        u32 cnt;
        u32 *cp;
        u8 *ccp;
        int sl, flag;
        struct E *base;
        u32 tbl[2];
        { u32 mk = 0x180000; p->flags |= mk; }
        cp = &p->cnt;
        ccp = &p->cc;
        cnt = *cp;
        if (cnt == 0) {
            s16 vec[3];
            struct { s16 a, b; } w;
            s32 a, b, t, xx, yy, zz;
            s16 *o;
            xx = p->x;
            yy = p->y;
            zz = p->z;
            vec[0] = xx - 256;
            vec[1] = yy;
            vec[2] = zz - 256;
            sub_081B9334(q->o534, &p->x, vec, 0, 1);
            sub_08021890(p, sub, sub + 0x60, 9, 2, cnt);
            o = (s16 *)(sub + 0x128);
            *(u32 *)&w = cnt;
            a = w.a;
            b = w.b;
            t = sub_08249274((a + b) << 5, 3) + 0x40;
            o[0] = t >= 0 ? t >> 1 : -(-t >> 1);
            o[1] = 0x40;
            t = sub_08249274((b - a) << 5, 3) + 0x40;
            o[2] = t >= 0 ? t >> 1 : -(-t >> 1);
            tbl[0] = gUnk_0824F788[0];
            tbl[1] = gUnk_0824F788[1];
            if (q->f5c7 > 1) {
                s32 v = RND() >> 3;
                s32 w2 = v >> 3;
                w2 <<= 3;
                sl = v - w2 + 0x14;
            } else {
                s32 v = RND() >> 3;
                s32 w2 = v >> 3;
                w2 <<= 3;
                sl = v - w2 + 0xc;
            }
            if (sub_081B658C(q->f56c) == 0)
                sl += 8;
            base = (struct E *)q->f56c;
            if (base[tbl[p->f5b4 & 1]].v > 0) {
                s32 v = RND() >> 3;
                s32 y;
                { s32 w2 = v >> 5; w2 <<= 5; y = *ccp + (v - w2) - 0x10; }
                sub_081B71D4(p->f57c, &p->x, p->f110, base[tbl[p->f5b4 & 1]].b4, y, sl, sub_08249350(RND() >> 3, 6));
            } else if (base[tbl[(p->f5b4 + 1) & 1]].v > 0) {
                s32 v = RND() >> 3;
                s32 y;
                { s32 w2 = v >> 5; w2 <<= 5; y = *ccp + (v - w2) - 0x10; }
                sub_081B71D4(p->f57c, &p->x, p->f110, base[tbl[(p->f5b4 + 1) & 1]].b4, y, sl, sub_08249350(RND() >> 3, 6));
            } else {
                s32 v = RND() >> 3;
                s32 y;
                { s32 w2 = v >> 5; w2 <<= 5; y = *ccp + (v - w2) - 0x10; }
                sub_081B70A8(p->f57c, &p->x, p->f110, y, sl, sub_08249350(RND() >> 3, 6));
            }
            sub_0822B2F8(0x583);
            (*cp)++;
        }
        {
            u8 v = *ccp;
            u8 *q = sub + 0xa8;
            int k = 0x17;
            if (v > 0x10) {
                k = 0x18;
                if (v > 0x2f) {
                    k = 0x19;
                    if (v > 0x40) {
                        k = 0x16;
                        if (v <= 0x9f)
                            k = 0x1a;
                    }
                }
            }
            sub_08214588(q, q + 0x2c);
            *(s16 *)(q + 0x10) = k;
        }
        sub_081B95E4(q->o534);
        (*cp)++;
        flag = 0;
        if (q->f55d == 2)
            flag = 1;
        if (flag) {
            p->f5b4++;
            GO(4);
        }
        break;
    }
    case 4: {
        int flag;
        s16 vec[3];
        u32 *cp = &p->cnt;
        p->flags |= 0x180000;
        if (*cp == 0) {
            vec[0] = p->x + 256;
            vec[1] = p->y;
            vec[2] = p->z + 256;
            if (p->f5c7 > 1)
                sub_081B9334(q->o534, &p->x, vec, 0, 2);
            else
                sub_081B9334(q->o534, &p->x, vec, 0, 4);
            (*cp)++;
        }
        sub_081B95E4(q->o534);
        flag = 0;
        if (q->f55d == 2)
            flag = 1;
        if (flag) {
            if (p->f5b4 > 3) {
                GO(5);
            } else {
                GO(2);
            }
        }
        break;
    }
    case 5: {
        u32 *cp = &p->cnt;
        u32 cnt = *cp;
        if (cnt == 0) {
            struct Q *qq;
            sub_08021890(p, sub, sub + 0x60, 8, 2, 2);
            (*cp)++;
            qq = (struct Q *)((u8 *)q + 0x3fc);
            sub_08220C8C(q->o444, qq->f454, 0, 0, 4);
            q->f500 = 1;
            qq->f458 = cnt;
        }
        if (p->f_b2 != 0) {
            GO(6);
        }
        break;
    }
    case 6:
        p->f_af = 1;
        break;
    }
}

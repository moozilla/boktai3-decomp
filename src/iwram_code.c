/* ARM C copied to IWRAM 0x03000000 by AgbMain (ROM 08000350-0800145C). */
// COMPILER: agbcc_arm
// CFLAGS: -O2 -mthumb-interwork
#include "global.h"

struct PaletteBank {
    u32 words[8];
};

extern s32 gUnk_03004DA4;
extern u16 gUnk_030051F0[];
extern struct PaletteBank gUnk_03004DC0[];

/* Same cache as Palette_CacheObjectBank, with a word copy instead of CpuFastSet. */
u32 Palette_CacheObjectBankArm(u32 key, u32 *src)
{
    s32 i = 0;
    for (; i < gUnk_03004DA4; i++)
        if (gUnk_030051F0[i] == key)
            goto found;
    if (gUnk_03004DA4 > 15)
        goto overflow;
    {
        s32 n = gUnk_03004DA4;
        u32 *dst = gUnk_03004DC0[n].words;
        dst[0] = src[0];
        gUnk_030051F0[n] = key;
        dst[1] = src[1];
        dst[2] = src[2];
        dst[3] = src[3];
        dst[4] = src[4];
        dst[5] = src[5];
        dst[6] = src[6];
        dst[7] = src[7];
        return gUnk_03004DA4++;
    }
found:
    return i;
overflow:
    return 0;
}

struct IwVec {
    s16 x, y, z;
};

struct IwSpr;

struct IwObj {
    u8 pad0[8];
    u32 flags;
    u8 padC[0xE];
    u8 priority;
    u8 pad1B[5];
    struct IwVec pos;
    u16 dx;
    u16 dy;
    s16 right;
    s16 bottom;
    s16 left;
    s16 top;
    u8 scaleX;
    u8 scaleY;
    u16 angle;
    u16 count;
    u16 palBase;
    u32 attr;
    u8 pad40[8];
    u8 *palSrc;
    struct IwSpr *spr;
    u8 *tiles;
    u8 pad54[8];
    struct IwObj *next;
};

extern u32 gUnk_03003A50;
extern u32 gUnk_03004290;
extern struct IwObj *gUnk_030042C8[];
extern u16 gUnk_03005418[];
void sub_08000430(struct IwObj *, s32, s32, s32);

static inline s32 IwProjectScale(s32 value)
{
    if (value >= 0)
        return value >> 5;
    return -((-value) >> 5);
}

static inline void IwProject(struct IwVec *screen, struct IwVec *world)
{
    s32 x = world->x;
    s32 z = world->z;
    s32 sum, vertical, scaledSum;
    screen->x = IwProjectScale(3 * (x - z));
    scaledSum = IwProjectScale(3 * (x + z));
    sum = scaledSum;
    vertical = IwProjectScale(3 * world->y);
    screen->x = (u16)screen->x - gUnk_03005418[0] + 120;
    screen->y = sum - vertical - gUnk_03005418[1] + 80;
    screen->z = sum + vertical - gUnk_03005418[2];
}

#ifdef NONMATCHING
// fuzzy: 55.4%
struct IwSpr {
    u8 flags;
    u8 size;
    s16 dx;
    s16 dy;
    u16 tile;
};

struct IwAffineParam {
    u8 angle;
    s8 a;
    s8 b;
    u8 pad;
};

struct IwTileReq {
    u8 *src;
    u32 desc;
};

struct IwOamOut {
    u32 attr01;
    u16 attr2;
    u16 attr3;
};

extern u8 gUnk_03005230;
extern u32 gUnk_0300522C;
extern s32 gUnk_03004BA0;
extern struct IwAffineParam gUnk_03004310[];
extern s16 gUnk_086149C4[];
extern u8 gUnk_03004D20[];
extern u8 gUnk_03004D90[];
extern u32 gUnk_03004D50[];
extern u8 gUnk_03004D30[];
extern u8 gUnk_03004D40[];
extern u8 gUnk_03004D10[];
extern s16 gUnk_030042E8;
extern u16 gUnk_030042DC;
extern u32 gUnk_03004304;
extern struct IwTileReq gUnk_02039000[];
extern s32 gUnk_03004798;
extern struct IwOamOut gUnk_03004390[];
extern u32 gUnk_03003A04;
extern s32 gUnk_0300479C;
extern struct IwOamOut gUnk_030047A0[];
extern u16 gUnk_03004790[];
u32 Palette_CacheObjectBankArm(u32 key, u32 *src);

static inline s32 IwScale6(s32 v)
{
    return v >= 0 ? v >> 6 : -((-v) >> 6);
}

static inline s32 IwScale12(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

static inline s32 IwAddAffine(u8 angle, s32 a, s32 b)
{
    s32 n = gUnk_03004BA0;
    if (n > 0x1f)
        return 0;
    gUnk_03004BA0 = n + 1;
    gUnk_03004310[n].angle = angle;
    gUnk_03004310[n].a = a;
    gUnk_03004310[n].b = b;
    return n;
}


#define IW_QUEUE_TILES(r, obj, tile, step, tw, th)     \
    if (gUnk_030042E8 + (step) >= 0x400)               \
        r = -1;                                        \
    else {                                             \
        u32 slot;                                      \
        u32 off;                                       \
        u8 *src;                                       \
        u8 *b;                                         \
        u8 *q;                                         \
        slot = gUnk_03004304;                          \
        r = 0;                                         \
        src = (obj)->tiles + ((tile) << 5);            \
        b = ewram + 0x39000;                           \
        off = slot << 3;                               \
        *(u8 **)(b + off) = src;                       \
        q = b + off;                                   \
        *(u32 *)(q + 4) = ((tw) << 5) | ((th) << 16);  \
        gUnk_03004304 = slot + 1;                      \
    }

static inline void IwPushOam(u32 attr01, u32 attr2)
{
    s32 c = gUnk_03004798;
    if (c <= 0x7f) {
        gUnk_03004798 = c + 1;
        gUnk_03004390[c].attr01 = attr01;
        gUnk_03004390[c].attr2 = attr2;
        gUnk_03003A04 |= 1;
    }
}

static inline void IwPushOamZ(u32 attr01, u32 attr2, s32 z, u32 prio)
{
    s32 c = gUnk_0300479C;
    if (c <= 0x7f) {
        struct IwOamOut *o = &gUnk_030047A0[c];
        gUnk_0300479C = c + 1;
        o->attr01 = attr01;
        o->attr2 = attr2;
        o->attr3 = (z + 0x80) << 8;
        gUnk_03004790[prio]++;
    }
}

// confidence=low: batches an IwObj's sprites into OAM (affine setup, bounds check, palette cache, tile requests)
void sub_08000430(struct IwObj *obj, s32 x, s32 y, s32 z)
{
    u32 oam[64];
    u8 attr3[64];
    u16 tile[64];
    u8 keyIdx[64];
    u32 keys[16];
    u8 affTbl[4];
    u32 bankIdx[16];
    s32 i;
    s32 n = 0;
    s32 nKeys = 0;
    s32 sinv, cosv;
    u32 fl2;
    u32 f4, f8;
    struct IwSpr *p;
    s32 j;
    u8 *ewram;
    s32 step;

    f4 = obj->flags & 4;
    f8 = obj->flags & 8;
    z = z >= 0 ? z >> 1 : -((-z) >> 1);
    if (z < -0x80)
        z = -0x80;
    else if (z > 0x7f)
        z = 0x7f;

    fl2 = 0;
    if (((gUnk_03005230 & 1) && obj->priority != 0 && !(obj->flags & 0x10000)) ||
        (obj->flags & 0x800))
        fl2 |= 0x1000;
    if (gUnk_0300522C != 0) {
        if (obj->flags & 0x4000)
            fl2 |= 0x400;
    }
    if (obj->flags & 0x100)
        fl2 |= 0x800;

    if (!(obj->flags & 2)) {
        sinv = cosv = 0;
    } else {
        s32 a = obj->scaleX;
        s32 b = obj->scaleY;
        s32 ra, rb, idx;

        if (f4)
            a = -a;
        if (f8)
            b = -b;
        if (obj->flags & 0x20000) {
            fl2 |= 0x100;
        } else {
            if ((a < 0 ? -a : a) > 0x3c || (b < 0 ? -b : b) > 0x3c || obj->angle != 0)
                fl2 |= 0x300;
            else
                fl2 |= 0x100;
        }
        affTbl[0] = IwAddAffine(obj->angle, (s8)a, (s8)b);
        if (obj->flags & 0x8000) {
            affTbl[1] = IwAddAffine(obj->angle, (s8)-a, (s8)b);
            affTbl[2] = IwAddAffine(obj->angle, (s8)a, (s8)-b);
            affTbl[3] = IwAddAffine(obj->angle, (s8)-a, (s8)-b);
        } else {
            affTbl[1] = affTbl[0];
            affTbl[2] = affTbl[0];
            affTbl[3] = affTbl[0];
        }
        idx = (u16)obj->angle;
        sinv = gUnk_086149C4[(-idx) & 0xff];
        cosv = gUnk_086149C4[(-idx + 0x40) & 0xff];
    }

    p = obj->spr;
    for (i = 0; i < obj->count; i++, p++) {
        u32 sz = p->size;
        s32 wd = gUnk_03004D90[sz];
        s32 ht = gUnk_03004D20[sz];
        u32 fl = fl2;
        s32 px, py;

        if (!(obj->flags & 2)) {
            s32 x1, y1;
            px = p->dx;
            py = p->dy;
            x1 = px + wd;
            y1 = py + ht;
            if (f4) {
                s32 t = px;
                px = -x1;
                x1 = -t;
            }
            if (f8) {
                s32 t = py;
                py = -y1;
                y1 = -t;
            }
            px += x;
            py += y;
            if (f4 != (p->flags & 4))
                fl |= 0x10000000;
            if (f8 != (p->flags & 8))
                fl |= 0x20000000;
            if (!(obj->flags & 0x40)) {
                x1 += x;
                y1 += y;
                if (x1 < 0)
                    continue;
                if (px > 0xF0)
                    continue;
                if (y1 < 0)
                    continue;
                if (py > 0xA0)
                    continue;
            }
        } else {
            s32 hx = wd >> 1;
            s32 hy = ht >> 1;
            s32 sx = IwScale6((p->dx + hx) * obj->scaleX);
            s32 sy = IwScale6((p->dy + hy) * obj->scaleY);
            s32 ey, rx, ry;
            u32 idx;

            if (f4)
                sx = -sx;
            ey = sy;
            if (f8)
                ey = -sy;
            if (p->flags & 4)
                idx = 1;
            else
                idx = 0;
            if (p->flags & 8)
                idx |= 2;
            fl |= (u32)affTbl[idx] << 25;
            rx = IwScale12(sx * cosv - ey * sinv);
            px = rx - hx;
            ry = IwScale12(sx * sinv + ey * cosv);
            py = ry - hy;
            if ((fl & 0x300) == 0x300) {
                s32 h2 = ht * 2;
                px = px - hx;
                py = py - hy;
                px += x;
                py += y;
                if (px + wd * 2 < 0)
                    continue;
                if (px > 0xF0)
                    continue;
                if (py + h2 < 0)
                    continue;
                if (py > 0xA0)
                    continue;
            } else {
                px += x;
                py += y;
                if (px + wd < 0)
                    continue;
                if (px > 0xF0)
                    continue;
                if (py + ht < 0)
                    continue;
                if (py > 0xA0)
                    continue;
            }
        }

        {
            u32 key;
            tile[n] = p->tile & 0xfff;
            key = p->tile >> 12;
            for (j = 0; j < nKeys && keys[j] != key; j++)
                ;
            keyIdx[n] = j;
            if (j == nKeys) {
                nKeys = j + 1;
                keys[j] = key;
            }
            attr3[n] = sz;
            oam[n] = (fl | ((gUnk_03004D50[sz] | obj->attr) | ((py & 0xff) | ((px & 0x1ff) << 16))));
            n++;
        }
    }

    ewram = (u8 *)0x2000000;
    if (n <= 0)
        return;

    {
        u8 *pal = obj->palSrc;
        for (j = 0; j < nKeys; j++)
            bankIdx[j] = Palette_CacheObjectBankArm(obj->palBase + keys[j], (u32 *)(pal + (keys[j] << 5)));
    }

    if (obj->flags & 0x20) {
        for (j = 0; j < n; j++) {
            u32 s = attr3[j];
            u32 t = tile[j];
            s32 tw = gUnk_03004D40[s];
            s32 th = gUnk_03004D10[s];
            s32 r;
            step = gUnk_03004D30[s];
            IW_QUEUE_TILES(r, obj, t, step, tw, th);
            if (r < 0)
                return;
            IwPushOam(oam[j],
                      (u32)gUnk_030042E8 | (obj->priority << 10) | (bankIdx[keyIdx[j]] << 12));
            gUnk_030042DC += step;
            gUnk_030042E8 += step;
        }
    } else {
        for (j = 0; j < n; j++) {
            u32 s = attr3[j];
            u32 t = tile[j];
            s32 tw = gUnk_03004D40[s];
            s32 th = gUnk_03004D10[s];
            s32 r;
            step = gUnk_03004D30[s];
            IW_QUEUE_TILES(r, obj, t, step, tw, th);
            if (r < 0)
                return;
            IwPushOamZ(oam[j],
                       (u32)gUnk_030042E8 | (obj->priority << 10) | (bankIdx[keyIdx[j]] << 12), z,
                       obj->priority);
            gUnk_030042DC += step;
            gUnk_030042E8 += step;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08000430);
#endif
void sub_08000E90(void)
{
    struct IwObj *o;
    u32 mask;
    s32 x, y, z;
    struct IwVec s;

    mask = ((gUnk_03003A50 & 1) ? 0x200 : 0x400) + 1;
    for (o = gUnk_030042C8[gUnk_03004290]; o != 0; o = o->next) {
        if (o->flags & mask)
            continue;
        if (!(o->flags & 0x10)) {
            IwProject(&s, &o->pos);
            x = s.x + o->dx;
            y = s.y + o->dy;
            z = s.z;
        } else {
            x = o->pos.x + o->dx;
            y = o->pos.y + o->dy;
            z = o->pos.z;
        }
        if (o->flags & 0x40) {
            if (x + o->left < 0)
                continue;
            if (y + o->top < 0)
                continue;
            if (x + o->right > 0xF0)
                continue;
            if (y + o->bottom > 0xA0)
                continue;
        }
        sub_08000430(o, x, y, z);
    }
}

void sub_08001038(void)
{
    struct IwObj *o;
    u32 mask;
    s32 x, y, z;

    mask = ((gUnk_03003A50 & 1) ? 0x200 : 0x400) + 1;
    for (o = gUnk_030042C8[gUnk_03004290]; o != 0; o = o->next) {
        if (o->flags & mask)
            continue;
        x = (s16)(o->pos.x + o->dx);
        y = (s16)(o->pos.y + o->dy);
        z = o->pos.z;
        if (!(o->flags & 0x40)) {
            if (x + o->left < 0)
                continue;
            if (y + o->top < 0)
                continue;
            if (x + o->right > 0xF0)
                continue;
            if (y + o->bottom > 0xA0)
                continue;
        }
        sub_08000430(o, x, y, z);
    }
}

void sub_08001120(void)
{
    struct IwObj *o;
    u32 mask;
    s32 x, y, z;
    struct IwVec s;

    mask = ((gUnk_03003A50 & 1) ? 0x200 : 0x400) + 1;
    for (o = gUnk_030042C8[gUnk_03004290]; o != 0; o = o->next) {
        if (o->flags & mask)
            continue;
        if (!(o->flags & 0x1000))
            continue;
        if (!(o->flags & 0x10)) {
            IwProject(&s, &o->pos);
            x = (s16)(s.x + o->dx);
            y = (s16)(s.y + o->dy);
            z = s.z;
        } else {
            x = (s16)(o->pos.x + o->dx);
            y = (s16)(o->pos.y + o->dy);
            z = o->pos.z;
        }
        if (!(o->flags & 0x40)) {
            if (x + o->left < 0)
                continue;
            if (y + o->top < 0)
                continue;
            if (x + o->right > 0xF0)
                continue;
            if (y + o->bottom > 0xA0)
                continue;
        }
        sub_08000430(o, x, y, z);
    }
}

void sub_080012E0(void)
{
    struct IwObj *o;
    u32 mask;
    s32 x, y, z;
    struct IwVec s;

    mask = ((gUnk_03003A50 & 1) ? 0x200 : 0x400) + 1;
    for (o = gUnk_030042C8[gUnk_03004290]; o != 0; o = o->next) {
        if (o->flags & mask)
            continue;
        if (!(o->flags & 0x2000))
            continue;
        if (!(o->flags & 0x10)) {
            IwProject(&s, &o->pos);
            x = (s16)(s.x + o->dx);
            y = (s16)(s.y + o->dy);
            z = s.z;
        } else {
            x = (s16)(o->pos.x + o->dx);
            y = (s16)(o->pos.y + o->dy);
            z = o->pos.z;
        }
        sub_08000430(o, x, y, z);
    }
}

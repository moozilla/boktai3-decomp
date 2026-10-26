#include "global.h"
#include "engine.h"

// confidence=low: action phases push neighboring scene objects and emit position effects.

#ifdef NONMATCHING
// fuzzy: 89.5%
// fix.py best (sweep after the inlined-insn fix)
// foundry draft (worker best 91.4%); what differs is mostly register allocation and instruction order
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct Cell {
    u16 unk0;
    u16 flags;
};
struct Q {
    u16 pad;
    u16 flags;
};
struct P {
    u8 pad0[0x20];
    u32 flags;
    u8 pad24[0x30 - 0x24];
    struct PackedVec pos;
    s16 vx, pad3a, vz;
    u8 pad3e[0x98 - 0x3e];
    u16 a98;
    u8 pad9a[0xa0 - 0x9a];
    u16 aa0;
    u8 pada2[0x348 - 0xa2];
    struct Q *q348;
    s16 target;
    u8 pad34e[0x3a0 - 0x34e];
    u16 a3a0;
    u8 a3a2, a3a3, a3a4;
    u8 pad3a5[0x418 - 0x3a5];
    u8 type;
    u8 pad419[0x458 - 0x419];
    u8 state;
    u8 pad459[0x4a4 - 0x459];
    u8 a4a4, dir;
    u8 pad4a6[0x534 - 0x4a6];
    u16 a534;
    u8 pad536[2];
    u16 a538;
    u8 pad53a[0x7c8 - 0x53a];
    u8 a7c8;
};

struct Map {
    u32 pad;
    u8 *cells;
    u8 pad8[28];
    u16 rows[1];
};
extern const s16 gUnk_086149C4[];
void Sound_PlaySong(u32 song);
s32 SceneObjects_TryPushNeighborByIndex(void *position, u32 index);
s32 Player_SetAnimation(void *, s32, s32, s32);
void ActorSequence_Begin(void *, s32, s32);
void Player_SetActionPhase(void *p, u8 v);
u32 Player_IsStatusBit3ActionActive(u32 x);
s32 ActorVariant_GetTableValue();
void Player_PlayPositionEffect(void *, void *, u32, u32, void *, u32, u32);
s32 SceneObjects_HasTypeTwoResource(s32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
s32 Player_IsWithinCoordinateBand(void *);
s32 sub_08156DAC(void);

static inline int scale(int v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
static inline int tileIndex(struct PackedVec *v)
{
    int x = v->x >> 8, z = v->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u8 *cell(int i)
{
    u8 *p = TerrainModifiers_FindCellOverride(i, 1);
    if (p)
        return p + 4;
    return gTerrainMap->cells + 12 + i * 4;
}
static inline int heightAt(struct PackedVec *v, int i)
{
    u8 *c = cell(i);
    int slope = *c >> 4;
    int h = (*c & 15) << 8;
    switch (slope) {
        case 1:
            h -= (u8)v->z;
            break;
        case 2:
            h -= (u8)v->x;
            break;
    }
    return h;
}

static inline void orFlags(u32 *a, u32 m)
{
    *a |= m;
}
static inline int isTwo(struct P *p)
{
    int r;
    u16 q = p->a538;
    r = 0;
    if (q != 0 && p->a534 == 2)
        r = 1;
    return r;
}

void Player_UpdateObjectPush(struct P *p)
{
    s32 sb;
    u8 state;
    struct PackedVec v;
    struct PackedVec a;
    struct PackedVec b;
    struct PackedVec c;
    if (!sub_08156DAC()) {
        ActorSequence_Begin(p, 0, 0);
        return;
    }
    if ((p->state == 1) && (p->state & p->q348->flags))
        Player_SetActionPhase(p, 2);
    state = p->state;
    switch (state) {
        case 0:
            orFlags(&p->flags, 4);
            sb = ActorVariant_GetTableValue(p->type, 2);
            if (p->a3a0 != sb) {
                Sound_PlaySong(0xd0);
                p->a3a4 = (p->dir + 4) & 7;
                if (p->dir > 4) {
                    p->a3a2 = (8 - p->dir) >> 1;
                    p->a3a3 = 1;
                } else {
                    p->a3a2 = p->dir >> 1;
                    p->a3a3 = state;
                }
            }
            if (Player_SetAnimation(p, sb, 0x40, 1) == 0)
                return;
            Player_SetActionPhase(p, 1);
            return;

        case 1: {
            int t;
            int d4;
            int d5;
            s8 h;
            int h2;
            orFlags(&p->flags, 8);
            p->a4a4 = state;
            t = p->target;
            d4 = (p->dir + 7) & 7;
            d5 = (p->dir + 1) & 7;
            sb = ActorVariant_GetTableValue(p->type, 3);
            p->a7c8 = state;
            if ((t != d4) && (t != d5)) {
                if (Player_IsWithinCoordinateBand(p)) {
                    if (SceneObjects_TryPushNeighborByIndex(&p->pos, p->a3a4) > 0) {
                        if ((p != 0) && isTwo(p)) {
                            ActorSequence_Begin(p, 0, 0);
                        } else {
                            ActorSequence_Begin(p, 5, 0);
                            p->a4a4 = 0;
                        }
                    }
                }
            } else {
                int s;
                int idx;
                int d;
                d = p->dir;
                t = ((((t - d) * 2) + 8) + d) & 7;
                t = (t + 1) & 7;
                v = p->pos;
                v.x -= scale(gUnk_086149C4[((t << 5) + 0x40) & 0xff] * 50);
                s = t << 5;
                v.z -= scale(gUnk_086149C4[s & 0xff] * 50);
                idx = tileIndex(&v);
                h = heightAt(&v, tileIndex(&v));
                if ((h > (p->pos.y - 0x80)) && (h < (p->pos.y + 0x80))) {
                    if (!Player_IsStatusBit3ActionActive(((struct Cell *)cell(idx))->flags)) {
                        v.x -= scale(gUnk_086149C4[((((p->dir + 1) & 7) << 5) + 0x40) & 0xff] * 50);
                        v.z -= scale(gUnk_086149C4[((p->dir + 1) & 7) << 5] * 50);
                        h2 = heightAt(&v, tileIndex(&v));
                        if (h2 >= (h + 0xc8)) {
                            if (!(((struct Cell *)cell(tileIndex(&v)))->flags & 4)) {
                                int m;
                                u16 q;
                                sb = ActorVariant_GetTableValue(p->type);
                                q = p->aa0;
                                m = 0;
                                if (q) {
                                    m = 3;
                                    if (q == 2)
                                        m = 6;
                                }
                                p->vx = -scale(gUnk_086149C4[(s + 0x40) & 0xff] * m);
                                p->vz = -scale(gUnk_086149C4[s & 0xff] * m);
                            }
                        }
                    }
                }
            }
            Player_SetAnimation(p, sb, 0x40, 1);
            if (((p->a3a0 == ActorVariant_GetTableValue(p->type, 4)) && (p->aa0 == 0)) &&
                (p->a98 == 1))
                Sound_PlaySong(0xdb);
            return;
        }

        case 2: {
            int d;
            orFlags(&p->flags, 8);
            p->a7c8 = 1;
            if (!Player_SetAnimation(p, ActorVariant_GetTableValue(p->type, 5), 0x40, 1))
                return;
            a = p->pos;
            b = p->pos;
            d = p->dir;
            if (d == 1) {
                a.x += 0x1c;
                a.y += 0x2c;
                a.z -= 0x4c;
                b.x += 0x40;
                b.z -= 0x80;
            } else if (d == 3) {
                a.x -= 0x34;
                a.y += 0;
                a.z += 0;
                b.x += 0x80;
                b.z += 0x40;
            } else if (d == 5) {
                a.x += 0xc;
                a.y += 0x2c;
                a.z -= 0x20;
                b.x += 0x40;
                b.z += 0x80;
            } else if (d == 7) {
                a.x -= 0x40;
                a.y += 0x44;
                a.z += 0x2c;
                b.x -= 0x80;
                b.z += 0x40;
            }
            c = p->pos;
            c.x -= scale(gUnk_086149C4[((((p->dir + 1) & 7) << 5) + 0x40) & 0xff] * 50);
            c.z -= scale(gUnk_086149C4[((p->dir + 1) & 7) << 5] * 50);
            if (((struct Cell *)cell(tileIndex(&c)))->flags & 0x100)
                Player_PlayPositionEffect(p, &p->pos, 0x100, 1, &a, 1, 0xeb);
            else
                Player_PlayPositionEffect(p, &p->pos, 0x100, 1, &a, 0, 0xec);
            if (SceneObjects_HasTypeTwoResource(tileIndex(&b)))
                Sound_PlaySong(0x184);
            Player_SetActionPhase(p, 1);
            return;
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Player_UpdateObjectPush);
#endif

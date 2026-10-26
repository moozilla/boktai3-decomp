#include "global.h"
#include "render.h"
typedef struct {
    u32 pad[2], flags;
    u8 unkC[0xe];
    u8 layer;
    u8 unk1B[5];
    s16 x, y, z;
    u8 unk26[0x14];
    u16 unk3A;
    u8 unk3C[0xc];
    void *unk48;
    u8 tail[0x14];
} Graphic;
typedef struct {
    s32 a, b;
} Pair;
typedef struct {
    u8 unk0[0xf];
    u8 unkF;
    u8 unk10[8];
    s16 x, y, z;
    u8 unk1E[0xa];
} Marker;
extern u8 gUnk_0824DB28[];
s32 Anim_LoadKeyframe(void *anim, void *data, u32 index, u32 mode);
void Anim_SetKeyframes(void *anim, void *data, u32 index, u32 mode, u32 flags);
void *AnimData_FindEntry(s32 id);
void Effect_SetOrigin(void *, s32, s32);
void SpriteDrawable_SelectTileFrame(void *, void *, u32);

typedef struct {
    u8 pad0[0x18];
    Pair p18;
    Pair p20;
    u8 pad28[0x40 - 0x28];
    s16 lo, hi;
    u16 mid, last;
    u8 pad48[0x84 - 0x48];
    u8 unk84[0xac - 0x84];
    u16 timer;
    u8 diff;
    u8 padaf[0xb7 - 0xaf];
    u8 layer;
    u8 padb8[0xe4 - 0xb8];
    u8 pal[0x104 - 0xe4];
    u8 anim[0x144 - 0x104];
    Graphic g[15];
    Marker marker[4];
} Ctx;
static inline void Clamp(s16 *v, s32 lo, s32 hi)
{
    if (*v < lo)
        *v = lo;
    else if (*v > hi)
        *v = hi;
}
s32 sub_0801BD44(Ctx *p)
{
    u32 layer = p->layer;
    s16 frame;
    u8 *tbl;
    u32 layer1 = layer + 1;
    s32 w;
    Graphic *g;
    void *entry;
    u8 *q;
    u8 *d;
    s32 i;
    s32 y;
    u32 zero;
    u32 z;
    u32 v0;
    s32 off;

    tbl = gUnk_0824DB28 + 5;
    switch (tbl[p->diff]) {
        case 10:
            w = 10;
            frame = 0xbe;
            break;
        case 20:
            w = 20;
            frame = 0xbf;
            break;
        case 30:
            w = 30;
            frame = 0xc0;
            break;
        default:
            w = 40;
            frame = 0xc1;
            break;
    }
    p->timer = 0xe10;
    p->mid = 100;
    p->lo = p->mid - w;
    p->hi = p->mid + w;
    Clamp(&p->lo, 0, 0xb0);
    Clamp(&p->hi, 0, 0xb0);
    p->last = 0;

    g = &p->g[0];
    g->x = 0;
    Anim_LoadKeyframe(g, p->anim, 0xb4, 0);
    g->flags &= ~1;
    g->layer = layer;

    g = &p->g[1];
    g->x = 0;
    Anim_LoadKeyframe(g, p->anim, 0xbd, 0);
    g->layer = layer;
    g->flags |= 1;

    g = &p->g[2];
    g->x = 0;
    Anim_LoadKeyframe(g, p->anim, 0xbc, 0);
    g->flags |= 1;
    g->layer = layer;

    g = &p->g[3];
    g->x = p->mid;
    Anim_LoadKeyframe(g, p->anim, frame, 0);
    g->flags |= 1;

    g = &p->g[4];
    g->x = p->last - 0xb0;
    Anim_LoadKeyframe(g, p->anim, 0xb8, 0);
    g->layer = layer1;
    g->flags &= ~1;
    CpuSet(gPaletteBankData + 0x75a0, p->pal, 0x04000008);
    g->unk3A = 0x8100;
    g->unk48 = p->unk84;

    g = &p->g[5];
    g->x = 0;
    Anim_SetKeyframes(g, p->anim, 1, 1, 0);
    g->flags &= ~1;

    g = &p->g[6];
    y = 0x20;
    for (i = 0; i < 5; i++, g++, y += 8) {
        g->x = 8;
        g->y = y;
        Anim_LoadKeyframe(g, p->anim, 0xc4, 0);
        g->layer = 0;
        g->flags |= 1;
    }
    p->p18 = p->p20;

    g = &p->g[11];
    Anim_LoadKeyframe(g, p->anim, 0xb6, 0);
    g->flags &= ~1;
    g->layer = layer1;

    g = &p->g[12];
    Anim_LoadKeyframe(g, p->anim, 0xce, 0);
    g->flags |= 1;
    g->layer = layer;

    g = &p->g[13];
    Anim_LoadKeyframe(g, p->anim, 0xb7, 0);
    g->flags &= ~1;
    g->layer = layer;

    entry = AnimData_FindEntry(0x1c1a);
    off = -8;
    v0 = 0;
    d = (u8 *)&p->marker[0].x;
    q = (u8 *)&p->marker[0];
    for (i = 3; i >= 0; i--) {
        Effect_SetOrigin(q, off, off);
        SpriteDrawable_SelectTileFrame(q, entry, 0x33);
        ((Marker *)q)->unkF = v0;
        zero = v0;
        CpuSet(&zero, d, 0x05000002);
        d += 0x28;
        q += 0x28;
    }
    z = 0;
    p->marker[0].x = z; /* overwritten at once, but needed to match */
    p->marker[0].x = 0x59;
    p->marker[0].y = 0x78;
    p->marker[0].z = z;

    g = &p->g[14];
    g->x = -8;
    g->y = -0x48;
    g->z = z;
    Anim_LoadKeyframe(g, p->anim, 0x3d, 0);
    g->flags |= 1;
    Anim_SetKeyframes(g, p->anim, 0, 1, z);
    g->layer = 0;
    return 0;
}

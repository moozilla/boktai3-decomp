#include "global.h"
#include "engine/video/render.h"
#include "engine/video/anim_sprite.h"
#include "engine/video/sprite_drawable_api.h"
typedef struct {
    s32 a, b;
} Pair;
extern u8 gUnk_0824DB28[];
s32 Anim_LoadKeyframe(void *anim, void *data, u32 index, u32 mode);
void Anim_SetKeyframes(void *anim, void *data, u32 index, u32 mode, u32 flags);
void *AnimData_FindEntry(s32 id);

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
    struct AnimSprite g[15];
    struct SpriteDrawable marker[4];
} Ctx;
static inline void Clamp(s16 *v, s32 lo, s32 hi)
{
    if (*v < lo)
        *v = lo;
    else if (*v > hi)
        *v = hi;
}
s32 ResultGame_InitializeDisplay(Ctx *p)
{
    u32 layer = p->layer;
    s16 frame;
    u8 *tbl;
    u32 layer1 = layer + 1;
    s32 w;
    struct AnimSprite *g;
    const struct SpriteEffectTileResource *entry;
    s32 i;
    s32 y;

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
    g->position.x = 0;
    Anim_LoadKeyframe(g, p->anim, 0xb4, 0);
    g->flags &= ~1;
    g->attribute1A = layer;

    g = &p->g[1];
    g->position.x = 0;
    Anim_LoadKeyframe(g, p->anim, 0xbd, 0);
    g->attribute1A = layer;
    g->flags |= 1;

    g = &p->g[2];
    g->position.x = 0;
    Anim_LoadKeyframe(g, p->anim, 0xbc, 0);
    g->flags |= 1;
    g->attribute1A = layer;

    g = &p->g[3];
    g->position.x = p->mid;
    Anim_LoadKeyframe(g, p->anim, frame, 0);
    g->flags |= 1;

    g = &p->g[4];
    g->position.x = p->last - 0xb0;
    Anim_LoadKeyframe(g, p->anim, 0xb8, 0);
    g->attribute1A = layer1;
    g->flags &= ~1;
    CpuSet(gPaletteBankData + 0x75a0, p->pal, 0x04000008);
    g->paletteBank = 0x8100;
    g->palette = (u16 *)p->unk84;

    g = &p->g[5];
    g->position.x = 0;
    Anim_SetKeyframes(g, p->anim, 1, 1, 0);
    g->flags &= ~1;

    g = &p->g[6];
    y = 0x20;
    for (i = 0; i < 5; i++, g++, y += 8) {
        g->position.x = 8;
        g->position.y = y;
        Anim_LoadKeyframe(g, p->anim, 0xc4, 0);
        g->attribute1A = 0;
        g->flags |= 1;
    }
    p->p18 = p->p20;

    g = &p->g[11];
    Anim_LoadKeyframe(g, p->anim, 0xb6, 0);
    g->flags &= ~1;
    g->attribute1A = layer1;

    g = &p->g[12];
    Anim_LoadKeyframe(g, p->anim, 0xce, 0);
    g->flags |= 1;
    g->attribute1A = layer;

    g = &p->g[13];
    Anim_LoadKeyframe(g, p->anim, 0xb7, 0);
    g->flags &= ~1;
    g->attribute1A = layer;

    entry = AnimData_FindEntry(0x1c1a);
    for (i = 0; i < 4; i++) {
        struct SpriteDrawable *q = &p->marker[i];
        Effect_SetOrigin(q, -8, -8);
        SpriteDrawable_SelectTileFrame(q, entry, 0x33);
        q->priority = 0;
        ZeroFill8(&q->position);
    }
    p->marker[0].position.x = 0x59;
    p->marker[0].position.y = 0x78;
    p->marker[0].position.z = 0;

    g = &p->g[14];
    g->position.x = -8;
    g->position.y = -0x48;
    g->position.z = 0;
    Anim_LoadKeyframe(g, p->anim, 0x3d, 0);
    g->flags |= 1;
    Anim_SetKeyframes(g, p->anim, 0, 1, 0);
    g->attribute1A = 0;
    return 0;
}

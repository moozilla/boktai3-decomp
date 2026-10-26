#include "global.h"
#include "render.h"
typedef struct {
    u32 pad[2], flags;
    u8 unkC[0xe];
    u8 layer;
    u8 unk1B[5];
    s16 x, y, z;
    u8 tail[0x3a];
} Graphic;
s32 Anim_LoadKeyframe(void *anim, void *data, u32 index, u32 mode);
void Anim_SetKeyframes(void *anim, void *data, u32 index, u32 mode, u32 flags);
s32 Anim_Step(void *anim, void *data);
void Bg_SetTilemapEntry(u32 n, s32 x, s32 y, u32 a, u32 b, u32 c);
void Text_DrawNumber(int, int, int, int, int, int, int);
s32 Div(s32, s32);
s32 Mod(s32, s32);

typedef struct {
    u8 pad0[0x40];
    s16 lo, hi;
    s16 mid, last;
    u8 state;
    u8 pad49[0x50 - 0x49];
    u32 count;
    u8 pad54[0x68 - 0x54];
    s32 pos;
    u8 pad6c[0x8c - 0x6c];
    s32 last32;
    u8 pad90[0x9c - 0x90];
    s32 mid32;
    u8 pada0[0xac - 0xa0];
    u16 timer;
    u8 padae[0xb1 - 0xae];
    u8 counter;
    u8 padb2;
    s8 fade;
    u8 padb4[0xb7 - 0xb4];
    u8 layer;
    u8 hideBar;
    u8 hideTime;
    u8 padba[0x102 - 0xba];
    u16 color;
    u8 anim[0x144 - 0x104];
    Graphic g[15];
} Ctx;
s32 ResultMenu_UpdateMarkers(Ctx *p);

static inline void Clamp(s32 *v)
{
    if (*v < 0)
        *v = 0;
    else if (*v > 0x1600)
        *v = 0x1600;
}

s32 sub_0801C24C(Ctx *p)
{
    s32 i;
    u32 t;
    s32 minutes, seconds;
    s32 k, inv;
    s32 r, gg, b;
    u16 *pal;
    Graphic *g;
    Graphic *g4;
    u32 zero;
    s32 last, lo;
    s16 *range;
    u8 *pHideBar, *pLayer, *pHideTime, *pCounter;
    s8 *pFade;

    if (p->pos > 0x1600)
        p->pos = 0x1600;
    else if (p->pos < 0)
        p->pos = 0;
    p->last = p->pos >> 5;
    Clamp(&p->last32);
    Clamp(&p->mid32);
    p->last = p->last32 >> 5;
    zero = 0;
    p->mid = p->mid32 >> 5;
    g4 = &p->g[4];
    g4->x = p->last - 0xb0;
    range = &p->lo;
    last = p->last;
    lo = *range;
    g4 = &p->g[5];
    if (last < lo) {
        if (p->state != 1) {
            p->state = 1;
            Anim_SetKeyframes(g4, p->anim, 1, 1, zero);
        }
    } else if (last < p->hi) {
        if (p->state != 2) {
            p->state = 2;
            Anim_SetKeyframes(g4, p->anim, 2, 1, zero);
        }
    } else if (p->state != 3) {
        p->state = 3;
        Anim_SetKeyframes(g4, p->anim, 3, 1, zero);
    }
    pHideBar = &p->hideBar;
    pLayer = &p->layer;
    pHideTime = &p->hideTime;
    pCounter = &p->counter;
    pFade = &p->fade;
    g = &p->g[0];
    for (i = 14; i >= 0; i--) {
        Anim_Step(g, p->anim);
        g++;
    }
    g4 = &p->g[3];
    g4->x = p->mid;
    if (*pHideBar) {
        for (i = -1; i <= 10; i++)
            Bg_SetTilemapEntry(*pLayer, i + 4, 0x12, 1, 0, 0xf);
    } else {
        for (i = 0; i < p->count; i++)
            Bg_SetTilemapEntry(*pLayer, i + 4, 0x12, 0x5d, 0, 0xd);
        for (; i <= 9; i++)
            Bg_SetTilemapEntry(*pLayer, i + 4, 0x12, 0x5e, 0, 0xd);
    }
    t = *pHideTime;
    if (t) {
        Bg_SetTilemapEntry(*pLayer, 0x15, 0x12, 1, 0, 0xf);
        Bg_SetTilemapEntry(*pLayer, 0x16, 0x12, 1, 0, 0xf);
        Bg_SetTilemapEntry(*pLayer, 0x17, 0x12, 1, 0, 0xf);
        Bg_SetTilemapEntry(*pLayer, 0x18, 0x12, 1, 0, 0xf);
        Bg_SetTilemapEntry(*pLayer, 0x19, 0x12, 1, 0, 0xf);
    } else {
        Div(Mod(p->timer, 60) * 100, 60);
        seconds = Div(p->timer, 60);
        minutes = Div(seconds, 60);
        seconds = Mod(seconds, 60);
        Text_DrawNumber(*pLayer, 0x15, 0x12, minutes, 0x11, 2, 0xd);
        Text_DrawNumber(*pLayer, 0x18, 0x12, seconds, 0x11, 2, 0xd);
        Bg_SetTilemapEntry(*pLayer, 0x17, 0x12, 0x2c, t, 0xd);
    }
    g4 = &p->g[12];
    if (*pCounter) {
        if (*pCounter <= 3)
            Anim_LoadKeyframe(g4, p->anim, 0xcf, 0);
        else
            Anim_LoadKeyframe(g4, p->anim, 0xd0, 0);
        if (++*pCounter > 10)
            *pCounter = 0;
    } else
        Anim_LoadKeyframe(g4, p->anim, 0xce, 0);
    pal = (u16 *)(gPaletteBankData + 0x75a0);
    k = *pFade;
    inv = 0x20 - k;
    r = inv * 31;
    r = (r + (pal[15] & 31) * k) >> 5;
    gg = (inv * 4 + ((pal[15] >> 5) & 31) * k) >> 5;
    b = (((pal[15] >> 10) & 31) * k) >> 5;
    p->color = (r & 31) + ((gg & 31) << 5) + ((b & 31) << 10);
    return ResultMenu_UpdateMarkers(p);
}

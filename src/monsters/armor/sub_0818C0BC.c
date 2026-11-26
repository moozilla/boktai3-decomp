#include "global.h"

s32 Script_GetValue(void);
s32 Script_SeekToKeyword(u32);
u32 TerrainOverlay_QueueAddActor(void *p);
u32 ScaledShadow_InitializeDefaultLarge(void *a, void *b);
s32 TargetRegistry_RegisterActor(void *n, void *v, void *w);
void Armor_InitPaletteController(void *s);
void Widget_InitLinked(void *, void *, int, int, int, int, int, void *);
void ProximityVolume_TranslateBounds(void *, void *);
void SceneBody_Init(void *, u32, void *, s32, s32, void *);
void SceneBody_SetKeyframeDisplay(void *, void *);
void Armor_InitCombatColliders(void *, u32);
void AnimData_Load(void *, u32);
void ResourceHeader_Relocate(void *, void *);
void *ScriptProc_Start(u32, u32);
u32 Script_GetKeywordValue(u32, u32);
void Anim_SetKeyframes(void *, void *, int, int, int);
void Armor_InitParts(void *, u32, u32);
void Armor_ReceiveTargetEvent(void);

struct T {
    u32 a : 16;
    u32 b : 16;
    u32 c : 16;
    u32 d : 16;
};

struct W {
    u32 w[8];
};

struct E {
    u32 a, b;
    u8 pad[0x38];
};

static inline void clr(struct E *e)
{
    e->a = 0;
    e->b = 0;
}

struct S {
    u8 pad0[0x18];
    u8 f18[0x24];
    u8 f3c[0x58];
    struct W f94;
    u8 fb4[0x20];
    struct T fd4;
    u8 pad1[0x114 - 0xdc];
    u8 f114[0x6c];
    u32 f180[2];
    u8 pad2[0x2e0 - 0x188];
    u8 f2e0[0x1c];
    u8 f2fc[0x1c];
    u8 f318[0x1c];
    u8 f334[0x1c];
    struct E e[12];
    u8 pad3[0xB18 - 0x650];
    u32 fB18;
    u16 hB1C;
    u16 hB1E;
    u16 hB20;
    u16 hB22;
    u16 hB24;
    u16 hB26;
    u16 hB28;
    u16 hB2A;
    u16 hB2C;
    u16 hB2E;
    u16 hB30;
    u16 hB32;
    u32 wB34;
    u8 bB38;
    u8 bB39;
    u8 bB3A;
    u8 bB3B;
    u8 bB3C;
    u8 bB3D;
    u8 bB3E;
    u8 bB3F;
    u8 bB40;
    u8 bB41;
    u8 bB42;
    u8 bB43;
    u8 bB44;
    u8 bB45;
    u16 hB46;
    u32 wB48;
    u32 wB4C;
    u32 wB50;
    u32 wB54;
    u8 fB58[0x1120 - 0xB58];
    u8 b1120;
    u8 b1121;
    u16 h1122;
    u16 h1124;
    u16 h1126;
    u16 h1128;
    u16 h112A;
    u16 h112C;
    u16 h112E;
    u16 h1130;
};

s32 Armor_InitFromScript(struct S *p, u32 arg)
{
    struct T t;
    void *q;
    u32 r;
    s32 i;
    struct E *e;
    u32 *n;
    u8 *a;
    u8 *w;
    u8 *x;
    struct T *y;
    u32 z0;
    u32 one;

    t.a = 0x480;
    t.b = 0x100;
    t.c = 0x480;
    p->wB34 = 0;
    p->bB38 = 0;
    p->hB32 = 0x8001;
    p->bB39 = 0;
    p->bB3A = 0;
    p->bB3C = 0x2c;
    p->bB3D = 0;
    p->bB3E = 0;
    p->bB3F = 0x2f;
    p->bB40 = 0;
    p->bB41 = 0;
    p->bB42 = 0;
    p->bB43 = 0;
    p->hB46 = 0;
    p->wB48 = 0;
    if (Script_SeekToKeyword(0x70)) {
        t.a = Script_GetValue();
        t.b = Script_GetValue();
        t.c = Script_GetValue();
    }
    p->bB39 = Script_GetKeywordValue(0x64, 0);
    p->hB1E = Script_GetKeywordValue(0x6c, 5000);
    p->hB1C = p->hB1E;
    p->hB22 = Script_GetKeywordValue(0x73, 3000);
    p->hB20 = p->hB22;
    p->hB24 = Script_GetKeywordValue(0x62, (s16)p->hB1E >> 1);
    p->bB38 = Script_GetKeywordValue(0x74, 0);
    p->wB4C = Script_GetKeywordValue(0x65, 0);
    p->wB50 = Script_GetKeywordValue(0x66, 0);
    p->wB54 = Script_GetKeywordValue(0x6d, 0);
    p->hB26 = Script_GetKeywordValue(0x41, 0);
    p->hB28 = Script_GetKeywordValue(0x44, 0);
    p->hB2A = Script_GetKeywordValue(0x45, 0);
    p->hB30 = Script_GetKeywordValue(0x53, 0);
    p->hB2C = 0;
    p->bB45 = 0;
    if (Script_SeekToKeyword(0x69))
        p->hB32 |= 0x800;
    q = ScriptProc_Start(0xCB05, 0x761E);
    p->f94 = *(struct W *)q;
    ResourceHeader_Relocate(&p->f94, q);
    Widget_InitLinked(p->fb4, &p->f94, 0, 0, 2, 0, 0x3c, &t);
    Anim_SetKeyframes(p->fb4, &p->f94, 0, 1, 0);
    clr(&p->e[0]);
    clr(&p->e[1]);
    clr(&p->e[2]);
    w = p->fb4;
    x = p->f3c;
    y = &p->fd4;
    a = p->f18;
    z0 = 0;
    e = &p->e[1];
    e += 2;
    i = 8;
    do {
        e->a = z0;
        e->b = z0;
        e++;
    } while (--i >= 0);
    AnimData_Load(p->f2e0, 0x24B9);
    AnimData_Load(p->f2fc, 0x24B9);
    AnimData_Load(p->f318, 0x24B9);
    AnimData_Load(p->f334, 0x24B9);
    Armor_InitPaletteController(p);
    SceneBody_Init(x, p->fB18, &t, 0, 4, p);
    TerrainOverlay_QueueAddActor(x);
    SceneBody_SetKeyframeDisplay(x, w);
    *y = t;
    n = p->f180;
    TargetRegistry_RegisterActor(n, x, Armor_ReceiveTargetEvent);
    one = 1;
    n[1] |= one;
    ScaledShadow_InitializeDefaultLarge(p->f114, y);
    ProximityVolume_TranslateBounds(a, &t);
    Armor_InitCombatColliders(p, arg);
    p->hB2E = 0;
    r = 0;
    switch (p->bB38) {
        case 0:
        case 1:
            p->b1120 = 0;
            p->h1122 = 0;
            p->h1124 = 0x8000;
            p->h1126 = 0x8000;
            p->h1128 = 0x8000;
            break;
        case 2:
        case 3:
            p->b1120 = 0;
            p->h1122 = 0;
            p->h1124 = 0;
            p->h1126 = 0;
            r = p->bB38 == 2 ? 1 : 3;
            break;
        case 4:
        case 5:
            p->b1120 = 0;
            p->h1122 = 0;
            p->b1121 = 0;
            p->h1124 = 0;
            p->h1126 = 0;
            p->h1128 = 0;
            r = p->bB38 == 4 ? 1 : 2;
            break;
        case 6:
        case 7:
        case 12:
        case 13:
            p->b1120 = 0;
            p->h1122 = 0;
            p->h1124 = 0;
            p->h1126 = 0;
            p->h1128 = 0;
            p->h112A = 0;
            p->h112C = p->hB26;
            p->h112E = p->hB28;
            p->h1130 = 0;
            r = 3;
            break;
        case 8:
        case 9:
        case 14:
        case 15:
            p->b1120 = 0;
            p->b1121 = 0;
            p->h1122 = 0;
            p->h1124 = 0x1A4;
            p->h1126 = 0;
            p->h1128 = 0;
            p->h112A = 0;
            p->h112C = p->hB26;
            p->h112E = p->hB28;
            r = 3;
            break;
        case 10:
            p->b1120 = 0;
            p->h1122 = 0;
            p->h1124 = 0;
            p->h1126 = 0;
            r = 3;
            break;
        case 11:
            p->b1120 = 0;
            p->b1121 = 0;
            p->h1122 = 0;
            p->h1124 = 0;
            p->h1126 = 0;
            p->h1128 = 0;
            r = 2;
            break;
    }
    Armor_InitParts(p->fB58, r, p->bB38);
    return 0;
}

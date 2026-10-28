#include "global.h"
#include "render.h"
struct ScreenParams3F8Params {
    u8 *mBlock;
    u8 *eBlock;
    s32 b;
    s32 d;
    s32 R;
    s32 t;
    s32 c;
    s32 B;
    s32 f;
    s32 M;
};
struct SpWidget {
    u8 pad0[8];
    u32 flags;
    u8 padC[0x2E];
    u16 x3A;
    u8 pad3C[0xC];
    void *pal;
    u8 pad4C[0x14];
};
struct SpRes {
    u32 words[8];
};
struct SpDisplay {
    u8 pad0[0x1C];
    s32 x1C;
    u8 pad20[4];
    s32 x24;
    struct SpRes res;
    struct SpWidget w48;
    struct SpWidget wA8;
    s32 x108;
    u8 pad10C[8];
    s32 x114;
    void *x118;
    u16 palette[16];
    u8 pad13C[8];
    s32 x144;
    s32 x148;
    s32 x14C[16];
    s32 x18C[16];
    s32 x1CC[16];
    s32 x20C;
    s32 x210;
};
struct Obj53F8 {
    u8 pad0[0x10];
    s32 x10;
};
extern struct ScreenParams3F8Params gScreenTextParameters;
extern s32 gScreenTextSavedValue;
extern struct Obj53F8 *gUnk_030053F8;
s32 ScreenParams3F8_SelectValidDataBank(struct SpDisplay *);
void Bg_ClearTilemap(s32);
void BgGfx_LoadTilemapSimple(s32, s32, s32, s32, s32);
void *ScriptProc_Start(u16, u16);
void Bg_SetupLayers(s32, s32, void *, s32, s32, s32, s32 *);
void ResourceHeader_Relocate(void *, void *);
void Widget_InitLinked(void *, void *, s32, s32, s32, s32, s32, void *);
void Menu_EraseRect(s32, s32, s32, s32);
void Menu_DrawStringRef(s32, s32, u8 *, s32);
void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_RequestShow(void);
void ScrollingDialogue_SetScript(u8 *);
s32 Script_ParseStringRef(const void *);
u8 *Text_LookupString(s32);
void Menu_StartFadeIn(s32);

s32 sub_081EBD0C(struct SpDisplay *p)
{
    struct SpRes *res;
    struct SpWidget *w1;
    struct SpWidget *w2;
    u32 *flags;
    void **pal;
    u16 *x3A;
    s32 i;
    s32 opt;
    void *gfx;

    if (gScreenTextParameters.B == 1)
        p->x1C = gScreenTextSavedValue;
    if (gScreenTextParameters.M != 0) {
        if (gScreenTextParameters.M == 1)
            p->x24 = 4;
        else
            p->x24 = 3;
    } else if (gUnk_030053F8->x10 == 0x369F)
        p->x24 = 3;
    else
        p->x24 = 2;
    if (p->x1C > p->x24)
        p->x1C = p->x24;
    p->x14C[0] = 0x200;
    p->x14C[1] = 0x200;
    p->x14C[2] = 0x200;
    p->x14C[3] = 0x200;
    p->x14C[4] = 0x200;
    p->x14C[5] = 0x100;
    p->x14C[6] = 0x100;
    p->x14C[7] = 0x100;
    p->x14C[8] = 0x100;
    p->x14C[9] = 0x100;
    p->x14C[10] = 0x200;
    p->x14C[11] = 0x100;
    p->x14C[12] = 0x200;
    p->x14C[13] = 0x100;
    p->x14C[14] = 4;
    p->x14C[15] = 8;
    res = &p->res;
    w1 = &p->w48;
    w2 = &p->wA8;
    flags = &p->wA8.flags;
    pal = &p->wA8.pal;
    x3A = &p->wA8.x3A;
    for (i = 0; i < 16; i++) {
        p->x18C[i] = 0;
        p->x1CC[i] = 0;
    }
    p->x144 = 0;
    if (gScreenTextParameters.f == 0)
        p->x210 = 600;
    else
        p->x210 = gScreenTextParameters.f;
    if (gScreenTextParameters.M == 0 && gScreenTextParameters.B == 0) {
        p->x148 = ScreenParams3F8_SelectValidDataBank(p);
        if (p->x148 == -1)
            p->x108 = 3;
    }
    Bg_ClearTilemap(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, 0);
    Bg_ClearTilemap(3);
    gfx = ScriptProc_Start(0xC091, 0xA597);
    if (p->x148 == -1)
        opt = 3;
    else
        opt = 0xF;
    Bg_SetupLayers(2, 0, gfx, 0, 0, 1, &opt);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
    CpuSet((u8 *)ScriptProc_Start(0x92B3, 0x81B) + 0x14, gBgPaletteBuffer, 0x100);
    ResourceHeader_Relocate(res, ScriptProc_Start(0xCB05, 0x530D));
    if (gScreenTextParameters.M)
        Widget_InitLinked(w1, res, 0xD9, 0x10, 0, 0, 0, 0);
    else
        Widget_InitLinked(w1, res, 0xD1, 0x10, 0, 0, 0, 0);
    Widget_InitLinked(w2, res, 0x47, 0x10, 0, 0, 0, 0);
    *flags |= 1;
    CpuSet(p->x118 = *pal, p->palette, 0x10);
    *pal = p->palette;
    {
        u16 v = 0xE010;
        *x3A = v;
    }
    if (p->x148 >= 0) {
        Menu_EraseRect(1, 4, 0x1C, 9);
        if (gScreenTextParameters.M != 0) {
            if (gScreenTextParameters.M == 1) {
                Menu_DrawStringRef(1, 4, gScreenTextParameters.mBlock, 0);
                Menu_DrawStringRef(1, 7, gScreenTextParameters.mBlock, 1);
                Menu_DrawStringRef(1, 10, gScreenTextParameters.mBlock, 2);
                Menu_DrawStringRef(1, 13, gScreenTextParameters.mBlock, 3);
            } else {
                Menu_DrawStringRef(1, 4, gScreenTextParameters.mBlock, 0);
                Menu_DrawStringRef(1, 7, gScreenTextParameters.mBlock, 2);
                Menu_DrawStringRef(1, 10, gScreenTextParameters.mBlock, 3);
            }
        } else {
            Menu_DrawStringRef(9, 4, gScreenTextParameters.mBlock, 0);
            Menu_DrawStringRef(10, 7, gScreenTextParameters.mBlock, 1);
            if (p->x24 == 3)
                Menu_DrawStringRef(5, 10, gScreenTextParameters.mBlock, 2);
        }
        ScrollingDialogue_ClearScript();
        ScrollingDialogue_RequestShow();
        ScrollingDialogue_ClearScript();
        {
            u8 *e = gScreenTextParameters.eBlock;
            s32 base = p->x1C;
            ScrollingDialogue_SetScript(Text_LookupString(Script_ParseStringRef(e) + base));
        }
    } else {
        Menu_EraseRect(1, 6, 0x1C, 6);
        Menu_DrawStringRef(1, 6, gScreenTextParameters.mBlock, 3);
        *(u32 *)((u8 *)p + 0x50) |= 1;
    }
    p->x114 = 3;
    Menu_StartFadeIn(3);
    return 0;
}

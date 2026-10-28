#include "global.h"
#include "render.h"
#include "ram.h"
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
    s32 x10C;
    s32 x110;
    s32 x114;
    void *x118;
    u16 palette[16];
    u8 pad13C[4];
    s32 x140;
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
extern struct Obj53F8 *gSessionState;
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

struct Pos {
    s16 x, y, z;
};
extern u16 gUnk_03005260[];
void ScrollingDialogue_RequestHide(void);
void Sound_PlaySong(s32);
void Menu_StartFadeOut(s32);
void ScreenText_ShowIndexedString(struct SpDisplay *);
void Menu_SetTextRect(void *, struct Pos *);
void Palette_UpdateHighlightPulse(void *, void *);
void Script_Call(u8 *, s32);
void Task_Kill(void *);

static inline void Confirm(struct SpDisplay *p)
{
    Sound_PlaySong(0xDD);
    gScreenTextSavedValue = p->x1C;
    Menu_StartFadeOut(p->x114);
}

// ScreenParams3F8 display update: confirm/cancel, cursor movement, and a
// 16-key input sequence that unlocks a fourth choice (sets 0x369F).
void sub_081EB8E0(struct SpDisplay *p)
{
    struct Pos pos;
    s32 idx, i;
    struct SpWidget *w;
    u32 *fl;
    s32 diff, t, lim;

    switch (p->x108) {
        case 0:
            if (gUnk_03005260[1] & 1) {
                ScrollingDialogue_RequestHide();
                if (p->x1C == 0) {
                    Confirm(p);
                    p->x108 = 1;
                    p->x10C = 1;
                    break;
                }
                if (p->x1C == 1) {
                    Confirm(p);
                    p->x108 = 1;
                    if ((*(struct ScreenParams3F8Params *)&gScreenTextParameters).M == 2)
                        p->x10C = 3;
                    else
                        p->x10C = 2;
                    break;
                }
                if (p->x1C == 2) {
                    Confirm(p);
                    p->x108 = 1;
                    if ((*(struct ScreenParams3F8Params *)&gScreenTextParameters).M == 2)
                        p->x10C = 4;
                    else
                        p->x10C = 3;
                    break;
                }
                if ((*(struct ScreenParams3F8Params *)&gScreenTextParameters).M != 0 && p->x1C == 3) {
                    Confirm(p);
                    p->x108 = 1;
                    p->x10C = 4;
                    break;
                }
            }
            if (gUnk_03005260[1] & 2) {
                Sound_PlaySong(0xDE);
                ScrollingDialogue_RequestHide();
                p->x114 = 3;
                Menu_StartFadeOut(3);
                p->x108 = 1;
                p->x10C = 0;
                break;
            }
            if (gUnk_03005260[1] & 0x40) {
                Sound_PlaySong(0xDC);
                p->x1C--;
                if (p->x1C < 0)
                    p->x1C = p->x24 - 1;
                ScrollingDialogue_ClearScript();
                ScreenText_ShowIndexedString(p);
            }
            if (gUnk_03005260[1] & 0x80) {
                Sound_PlaySong(0xDC);
                p->x1C++;
                if (p->x1C >= p->x24)
                    p->x1C = 0;
                ScrollingDialogue_ClearScript();
                ScreenText_ShowIndexedString(p);
            }
            if (gUnk_03005260[1] != 0) {
                p->x18C[p->x144] = gUnk_03005260[1];
                p->x1CC[p->x144] = p->x140;
                p->x144++;
                if (p->x144 > 15)
                    p->x144 = 0;
            }
            idx = p->x144;
            t = p->x1CC[idx];
            diff = p->x140 - t;
            lim = p->x210;
            w = &p->wA8;
            fl = &p->wA8.flags;
            if (diff < lim && p->x20C == 0) {
                i = 0;
                if (p->x18C[idx] == p->x14C[i]) {
                    s32 magic = 0x369F;
                    do {
                    loop:
                        if (i == 15) {
                            Sound_PlaySong(0x296);
                            p->x20C = 1;
                            p->x24 = 3;
                            gSessionState->x10 = magic;
                            Menu_DrawStringRef(5, 10, (*(struct ScreenParams3F8Params *)&gScreenTextParameters).mBlock, 2);
                        }
                    } while (0);
                    idx++;
                    if (idx > 15)
                        idx = 0;
                    if (++i <= 15 && p->x18C[idx] == p->x14C[i])
                        goto loop;
                }
            }
            pos.x = 0;
            pos.y = p->x1C * 24 + 32;
            pos.z = 0;
            Menu_SetTextRect(w, &pos);
            *fl &= ~1;
            Palette_UpdateHighlightPulse((u8 *)p + 0x134, (u8 *)p + 0x13C);
            break;
        case 1:
            if (p->x110 > (1 << p->x114) || p->x114 == 0) {
                p->wA8.flags |= 1;
                p->w48.flags |= 1;
                if (p->x10C == 0)
                    Script_Call((u8 *)(*(struct ScreenParams3F8Params *)&gScreenTextParameters).c, 0);
                else if (p->x10C == 1)
                    Script_Call((u8 *)(*(struct ScreenParams3F8Params *)&gScreenTextParameters).b, 0);
                else if (p->x10C == 2)
                    Script_Call((u8 *)(*(struct ScreenParams3F8Params *)&gScreenTextParameters).d, 0);
                else if ((*(struct ScreenParams3F8Params *)&gScreenTextParameters).M != 0) {
                    if (p->x10C == 4)
                        Script_Call((u8 *)(*(struct ScreenParams3F8Params *)&gScreenTextParameters).t, 0);
                    else
                        Script_Call((u8 *)(*(struct ScreenParams3F8Params *)&gScreenTextParameters).R, 0);
                } else
                    Script_Call((u8 *)(*(struct ScreenParams3F8Params *)&gScreenTextParameters).R, 0);
                Task_Kill(p);
            }
            p->x110++;
            break;
        case 2:
            break;
        case 3:
            if (p->x140 == 0)
                Sound_PlaySong(0x192);
            if ((gUnk_03005260[1] & 1) || (gUnk_03005260[1] & 2)) {
                Sound_PlaySong(0xDE);
                p->x114 = 3;
                Menu_StartFadeOut(3);
                p->x108 = 1;
                p->x10C = 0;
            }
            break;
    }
    p->x140++;
}

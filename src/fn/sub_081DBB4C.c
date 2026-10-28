#include "global.h"
#include "menu_parameters.h"
/* ScreenParams2E8_UpdateScreen: per-frame state machine (blinking prompt, solar baseline capture and save header write, result message, fade out, return to script) */

struct SaveScreenAnim {
    u32 unk0;
    u32 unk4;
    u32 flags;
    u8 pad[0x88 - 0xC];
};

struct SaveScreen {
    u8 pad0[0x18];
    u32 bgMap;
    u8 pad1C[0x24 - 0x1C];
    s32 blinkTimer;
    s32 pageTimer;
    s32 state;
    s32 page;
    s32 timer;
    s32 unk38;
    s32 fadeTimer;
    s32 fadeSpeed;
    struct SaveScreenAnim anims[3];
};

struct SaveScreenScriptArgs {
    u32 count : 16, unused : 16;
    u32 *values;
};

extern u16 gUnk_03005260[];
s32 Mod(s32, s32);
void Menu_SetBackgroundMap(u32, u32);
void Sound_PlaySong(u32);
s32 Solar_CaptureBaseline(void);
void Save_WriteHeader(void);
void Bg_ClearTilemap(u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void MessageWindow_Clear(void);
void MessageWindow_ShowMessage(u32);
void MessageWindow_SetBounds(u32, u32, u32, u32);
void Menu_StartFadeOut(u32);
s32 Script_Call(u32, void *);
void Task_Kill(void *);

void ScreenParams2E8_UpdateScreen(struct SaveScreen *s)
{
    u32 result;
    struct SaveScreenScriptArgs args;

    result = 0;
    args.count = 1;
    args.values = &result;
    switch (s->state) {
        case 0:
            if (Mod(s->blinkTimer, 30) == 0) {
                s32 off = s->page * 0x88;
                u8 *base = (u8 *)&s->anims[1].flags;
                u32 *flags = (u32 *)(base + off);
                u32 v = *flags;
                if (v & 1)
                    v &= ~1;
                else
                    v |= 1;
                *flags = v;
            }
            if (Mod(s->pageTimer, 180) == 0) {
                if (s->page == 0) {
                    Menu_SetBackgroundMap(s->bgMap, 7);
                    s->page = 1 - s->page;
                    s->anims[1].flags |= 1;
                    s->anims[2].flags &= ~1;
                } else {
                    Menu_SetBackgroundMap(s->bgMap, 6);
                    s->page = 1 - s->page;
                    s->anims[1].flags &= ~1;
                    s->anims[2].flags |= 1;
                }
            }
            s->pageTimer++;
            s->blinkTimer++;
            if ((gUnk_03005260[1] & 1) && s->timer > 30) {
                s32 off = s->page * 0x88;
                u8 *base = (u8 *)&s->anims[1].flags;
                *(u32 *)(base + off) &= ~1;
                s->timer = 0;
                s->state = 1;
            } else if (gUnk_03005260[1] & 2) {
                if (gMessageMenuParameters.f == 0 && gMessageMenuParameters.c != 0) {
                    Sound_PlaySong(0xDE);
                    s->state = 5;
                    s->unk38 = 0;
                }
            }
            break;
        case 1:
            if (s->timer <= 59) {
                if (Solar_CaptureBaseline() != 0) {
                    if (gMessageMenuParameters.f == 0)
                        Save_WriteHeader();
                    s->timer = 0;
                    s->state = 2;
                }
            } else {
                s->timer = 0;
                s->state = 3;
            }
            break;
        case 2:
            Bg_ClearTilemap(0);
            BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
            MessageWindow_Clear();
            MessageWindow_ShowMessage(1);
            MessageWindow_SetBounds(0x28, 5, 0x18, 2);
            Sound_PlaySong(0xDD);
            s->timer = 0;
            s->state = 4;
            break;
        case 3:
            Bg_ClearTilemap(0);
            BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
            MessageWindow_Clear();
            MessageWindow_ShowMessage(2);
            MessageWindow_SetBounds(0x26, 5, 0x18, 2);
            Sound_PlaySong(0x192);
            s->timer = 0;
            s->state = 4;
            break;
        case 4:
            if (s->timer > 59) {
                Menu_StartFadeOut(s->fadeSpeed);
                s->timer = 0;
                s->state = 5;
            }
            break;
        case 5:
            if (s->fadeTimer > (1 << s->fadeSpeed)) {
                Bg_ClearTilemap(0);
                BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
                MessageWindow_Clear();
                s->anims[1].flags |= 1;
                s->anims[2].flags |= 1;
                Script_Call(gMessageMenuParameters.o, &args);
                Task_Kill(s);
            }
            s->fadeTimer++;
            break;
        case 6:
            break;
    }
    s->timer++;
}

#include "mode_menu.h"
#include "game_state.h"
#include "session_save.h"
#include "ram.h"
#include "render.h"
#include "input.h"
#include "runtime.h"
#include "engine.h"

void ModeMenu_ShowDescription(struct ModeMenu *menu)
{
    union {
        struct ModeMenu *menu;
        s32 index;
    } value;
    value.menu = menu;
    if (gScreenTextParameters.mode != 2) {
        const void *text = gScreenTextParameters.text;
        value.index = value.menu->selection;
        ScrollingDialogue_SetScript(Text_LookupString(Script_ParseStringRef(text) + value.index));
    } else {
        value.index = value.menu->selection;
        if (value.index > 0) {
            const void *text = gScreenTextParameters.text;
            value.index++;
            ScrollingDialogue_SetScript(
                Text_LookupString(Script_ParseStringRef(text) + value.index));
        } else {
            ScrollingDialogue_SetScript(
                Text_LookupString(Script_ParseStringRef(gScreenTextParameters.text) + value.index));
        }
    }
}

u8 Save_LoadSelectedSlot(void);

u8 Save_LoadAlternateSlot(void);

s32 ModeMenu_LoadAvailableSave(struct ModeMenu *unused)
{
    s32 r;
    s16 flag;
    int failure;
    if (Save_LoadSelectedSlot()) {
        gGameState->flag86E = 1;
        r = 1;
    } else {
        u8 v = Save_LoadAlternateSlot();
        if (v) {
            gGameState->flag86E = 1;
            r = 0;
        } else {
            gGameState->flag86E = v;
            failure = -1;
            r = failure;
        }
    }
    flag = (s16)gGameState->flag86E;
    if (flag) {
        flag = (s16)gGameState->flag86E;
        GameState_BackupActiveRegions(flag);
    }
    return r;
}

static inline void Confirm(struct ModeMenu *p)
{
    Sound_PlaySong(0xDD);
    gScreenTextSavedValue = p->selection;
    Menu_StartFadeOut(p->fadeShift);
}

// ScreenParams3F8 display update: confirm/cancel, cursor movement, and a

// 16-key input sequence that unlocks a fourth choice (sets 0x369F).

void ModeMenu_Update(struct ModeMenu *p)
{
    struct ContactPosition pos;
    s32 historyIndex, i;
    struct SceneInputWidget *cursor;
    u32 *cursorFlags;
    s32 elapsed, firstFrame, timeout;

    switch (p->state) {
        case 0:
            if (gPadState[1] & 1) {
                ScrollingDialogue_RequestHide();
                if (p->selection == 0) {
                    Confirm(p);
                    p->state = 1;
                    p->result = 1;
                    break;
                }
                if (p->selection == 1) {
                    Confirm(p);
                    p->state = 1;
                    if (gScreenTextParameters.mode == 2)
                        p->result = 3;
                    else
                        p->result = 2;
                    break;
                }
                if (p->selection == 2) {
                    Confirm(p);
                    p->state = 1;
                    if (gScreenTextParameters.mode == 2)
                        p->result = 4;
                    else
                        p->result = 3;
                    break;
                }
                if (gScreenTextParameters.mode != 0 && p->selection == 3) {
                    Confirm(p);
                    p->state = 1;
                    p->result = 4;
                    break;
                }
            }
            if (gPadState[1] & 2) {
                Sound_PlaySong(0xDE);
                ScrollingDialogue_RequestHide();
                p->fadeShift = 3;
                Menu_StartFadeOut(3);
                p->state = 1;
                p->result = 0;
                break;
            }
            if (gPadState[1] & 0x40) {
                Sound_PlaySong(0xDC);
                p->selection--;
                if (p->selection < 0)
                    p->selection = p->choiceCount - 1;
                ScrollingDialogue_ClearScript();
                ModeMenu_ShowDescription(p);
            }
            if (gPadState[1] & 0x80) {
                Sound_PlaySong(0xDC);
                p->selection++;
                if (p->selection >= p->choiceCount)
                    p->selection = 0;
                ScrollingDialogue_ClearScript();
                ModeMenu_ShowDescription(p);
            }
            if (gPadState[1] != 0) {
                p->keyHistory[p->historyIndex] = gPadState[1];
                p->keyFrames[p->historyIndex] = p->frame;
                p->historyIndex++;
                if (p->historyIndex > 15)
                    p->historyIndex = 0;
            }
            historyIndex = p->historyIndex;
            firstFrame = p->keyFrames[historyIndex];
            elapsed = p->frame - firstFrame;
            timeout = p->unlockFrames;
            cursor = &p->cursor;
            cursorFlags = &p->cursor.flags;
            if (elapsed < timeout && p->unlocked == 0) {
                i = 0;
                if (p->keyHistory[historyIndex] == p->unlockKeys[i]) {
                    s32 magic = 0x369F;
                    do {
                    loop:
                        if (i == 15) {
                            Sound_PlaySong(0x296);
                            p->unlocked = 1;
                            p->choiceCount = 3;
                            ((struct SessionSave *)gSessionState)->magic = magic;
                            Menu_DrawStringRef(5, 10, gScreenTextParameters.menuText, 2);
                        }
                    } while (0);
                    historyIndex++;
                    if (historyIndex > 15)
                        historyIndex = 0;
                    if (++i <= 15 && p->keyHistory[historyIndex] == p->unlockKeys[i])
                        goto loop;
                }
            }
            pos.x = 0;
            pos.y = p->selection * 24 + 32;
            pos.z = 0;
            Menu_SetTextRect(cursor, &pos);
            *cursorFlags &= ~1;
            Palette_UpdateHighlightPulse(&p->palette[12], p->pulseState);
            break;
        case 1:
            if (p->fadeTimer > (1 << p->fadeShift) || p->fadeShift == 0) {
                p->cursor.flags |= 1;
                p->background.flags |= 1;
                if (p->result == 0)
                    Script_Call(gScreenTextParameters.cancelScript, 0);
                else if (p->result == 1)
                    Script_Call(gScreenTextParameters.firstScript, 0);
                else if (p->result == 2)
                    Script_Call(gScreenTextParameters.secondScript, 0);
                else if (gScreenTextParameters.mode != 0) {
                    if (p->result == 4)
                        Script_Call(gScreenTextParameters.fourthScript, 0);
                    else
                        Script_Call(gScreenTextParameters.thirdScript, 0);
                } else
                    Script_Call(gScreenTextParameters.thirdScript, 0);
                Task_Kill(p);
            }
            p->fadeTimer++;
            break;
        case 2:
            break;
        case 3:
            if (p->frame == 0)
                Sound_PlaySong(0x192);
            if ((gPadState[1] & 1) || (gPadState[1] & 2)) {
                Sound_PlaySong(0xDE);
                p->fadeShift = 3;
                Menu_StartFadeOut(3);
                p->state = 1;
                p->result = 0;
            }
            break;
    }
    p->frame++;
}

void ModeMenu_Destroy(struct ModeMenu *p)
{
    Anim_Remove(&p->cursor);
    Anim_Remove(&p->background);
}

s32 ModeMenu_Initialize(struct ModeMenu *p)
{
    struct SceneInputResource *res;
    struct SceneInputWidget *w1;
    struct SceneInputWidget *w2;
    u32 *flags;
    u16 **pal;
    u16 *x3A;
    s32 i;
    s32 opt;
    void *gfx;

    if (gScreenTextParameters.restoreSelection == 1)
        p->selection = gScreenTextSavedValue;
    if (gScreenTextParameters.mode != 0) {
        if (gScreenTextParameters.mode == 1)
            p->choiceCount = 4;
        else
            p->choiceCount = 3;
    } else if (((struct SessionSave *)gSessionState)->magic == 0x369F)
        p->choiceCount = 3;
    else
        p->choiceCount = 2;
    if (p->selection > p->choiceCount)
        p->selection = p->choiceCount;
    p->unlockKeys[0] = 0x200;
    p->unlockKeys[1] = 0x200;
    p->unlockKeys[2] = 0x200;
    p->unlockKeys[3] = 0x200;
    p->unlockKeys[4] = 0x200;
    p->unlockKeys[5] = 0x100;
    p->unlockKeys[6] = 0x100;
    p->unlockKeys[7] = 0x100;
    p->unlockKeys[8] = 0x100;
    p->unlockKeys[9] = 0x100;
    p->unlockKeys[10] = 0x200;
    p->unlockKeys[11] = 0x100;
    p->unlockKeys[12] = 0x200;
    p->unlockKeys[13] = 0x100;
    p->unlockKeys[14] = 4;
    p->unlockKeys[15] = 8;
    res = &p->resource;
    w1 = &p->background;
    w2 = &p->cursor;
    flags = &p->cursor.flags;
    pal = &p->cursor.palette;
    x3A = &p->cursor.paletteBank;
    for (i = 0; i < 16; i++) {
        p->keyHistory[i] = 0;
        p->keyFrames[i] = 0;
    }
    p->historyIndex = 0;
    if (gScreenTextParameters.unlockFrames == 0)
        p->unlockFrames = 600;
    else
        p->unlockFrames = gScreenTextParameters.unlockFrames;
    if (gScreenTextParameters.mode == 0 && gScreenTextParameters.restoreSelection == 0) {
        p->saveIndex = ModeMenu_LoadAvailableSave(p);
        if (p->saveIndex == -1)
            p->state = 3;
    }
    Bg_ClearTilemap(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, 0);
    Bg_ClearTilemap(3);
    gfx = ScriptProc_Start(0xC091, 0xA597);
    if (p->saveIndex == -1)
        opt = 3;
    else
        opt = 0xF;
    Bg_SetupLayers(2, 0, gfx, 0, 0, 1, &opt);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
    CpuSet(((struct ModeMenuPalette *)ScriptProc_Start(0x92B3, 0x81B))->colors, gBgPaletteBuffer,
           0x100);
    ResourceHeader_Relocate(res, ScriptProc_Start(0xCB05, 0x530D));
    if (gScreenTextParameters.mode)
        Widget_InitLinked(w1, res, 0xD9, 0x10, 0, 0, 0, 0);
    else
        Widget_InitLinked(w1, res, 0xD1, 0x10, 0, 0, 0, 0);
    Widget_InitLinked(w2, res, 0x47, 0x10, 0, 0, 0, 0);
    *flags |= 1;
    CpuSet(p->savedPalette = *pal, p->palette, 0x10);
    *pal = p->palette;
    {
        u16 v = 0xE010;
        *x3A = v;
    }
    if (p->saveIndex >= 0) {
        Menu_EraseRect(1, 4, 0x1C, 9);
        if (gScreenTextParameters.mode != 0) {
            if (gScreenTextParameters.mode == 1) {
                Menu_DrawStringRef(1, 4, gScreenTextParameters.menuText, 0);
                Menu_DrawStringRef(1, 7, gScreenTextParameters.menuText, 1);
                Menu_DrawStringRef(1, 10, gScreenTextParameters.menuText, 2);
                Menu_DrawStringRef(1, 13, gScreenTextParameters.menuText, 3);
            } else {
                Menu_DrawStringRef(1, 4, gScreenTextParameters.menuText, 0);
                Menu_DrawStringRef(1, 7, gScreenTextParameters.menuText, 2);
                Menu_DrawStringRef(1, 10, gScreenTextParameters.menuText, 3);
            }
        } else {
            Menu_DrawStringRef(9, 4, gScreenTextParameters.menuText, 0);
            Menu_DrawStringRef(10, 7, gScreenTextParameters.menuText, 1);
            if (p->choiceCount == 3)
                Menu_DrawStringRef(5, 10, gScreenTextParameters.menuText, 2);
        }
        ScrollingDialogue_ClearScript();
        ScrollingDialogue_RequestShow();
        ScrollingDialogue_ClearScript();
        {
            const void *e = gScreenTextParameters.text;
            s32 base = p->selection;
            ScrollingDialogue_SetScript(Text_LookupString(Script_ParseStringRef(e) + base));
        }
    } else {
        Menu_EraseRect(1, 6, 0x1C, 6);
        Menu_DrawStringRef(1, 6, gScreenTextParameters.menuText, 3);
        p->background.flags |= 1;
    }
    p->fadeShift = 3;
    Menu_StartFadeIn(3);
    return 0;
}

/* Mode menu scripts, restored selection and unlock-sequence timeout. */

void ModeMenu_SetParametersFromScript(void)
{
    if (Script_SeekToKeyword('m'))
        gScreenTextParameters.menuText = Script_GetPc();
    if (Script_SeekToKeyword('e'))
        gScreenTextParameters.text = Script_GetPc();
    if (Script_SeekToKeyword('b'))
        gScreenTextParameters.firstScript = Script_GetValue();
    if (Script_SeekToKeyword('d'))
        gScreenTextParameters.secondScript = Script_GetValue();
    if (Script_SeekToKeyword('R'))
        gScreenTextParameters.thirdScript = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        gScreenTextParameters.cancelScript = Script_GetValue();
    if (Script_SeekToKeyword('t'))
        gScreenTextParameters.fourthScript = Script_GetValue();
    if (Script_SeekToKeyword('B'))
        gScreenTextParameters.restoreSelection = Script_GetValue();
    if (Script_SeekToKeyword('f'))
        gScreenTextParameters.unlockFrames = Script_GetValue();
    if (Script_SeekToKeyword('M'))
        gScreenTextParameters.mode = Script_GetValue();
}

void *ModeMenu_Create(void)
{
    void *r = Task_Create(0xb, sizeof(struct ModeMenu));
    if (r != 0) {
        Task_SetCallbacks(r, ModeMenu_Update, ModeMenu_Destroy);
        if (ModeMenu_Initialize(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}

void ResultScreen_Update(struct ResultScreen *delay)
{
    if (delay->state == 1) {
        if (delay->elapsed > (1 << delay->fadeShift) || delay->fadeShift == 0) {
            Script_Call(gScreenDelayParameters.script, 0);
            Task_Kill(delay);
        }
        delay->elapsed++;
    }
}

void ResultScreen_Destroy(struct ResultScreen *p)
{
    Anim_Remove(&p->widgets[0].sprite);
}

s32 ResultScreen_Initialize(struct ResultScreen *s)
{
    u32 zero;
    u32 value;
    u32 bank;
    u32 size;
    void *a;
    void *p;
    void *q;
    u32 old;
    void *obj;
    void *obj2;
    void *obj3;
    u32 *ptr;
    u32 *save;
    u8 *dest;
    Bg_ClearTilemap(0);
    zero = 0;
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, zero);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, zero);
    Bg_ClearTilemap(3);
    a = ScriptProc_Start(0xC091, 0x3536);
    value = 11;
    Bg_SetupLayers(2, 0, a, 0, zero, 1, &value);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, zero);
    p = ((struct ModeMenuPalette *)ScriptProc_Start(0x92B3, 0x204))->colors;
    CpuSet(p, gBgPaletteBuffer, size = 0x100);
    bank = 0xCB05;
    q = ScriptProc_Start(bank, 0xE2AB);
    obj = &s->widgets[0].resource;
    ResourceHeader_Relocate(obj, q);
    Widget_InitLinked(&s->widgets[0].sprite, obj, 0x3C, 0x10, zero, zero, zero, zero);
    q = ScriptProc_Start(bank, 0x530D);
    obj2 = &s->widgets[1].resource;
    ResourceHeader_Relocate(obj2, q);
    Widget_InitLinked(&s->widgets[1].sprite, obj2, 0x80, 0x10, zero, zero, zero, zero);
    s->fadeShift = 3;
    return Menu_StartFadeIn(3);
}

void ResultScreen_SetParametersFromScript(void)
{
    if (Script_SeekToKeyword(0x72))
        gScreenDelayParameters.text = Script_GetPc();
    if (Script_SeekToKeyword(0x65))
        gScreenDelayParameters.script = Script_GetValue();
}

void *ResultScreen_Create(void)
{
    void *r = Task_Create(0xb, sizeof(struct ResultScreen));
    if (r != 0) {
        Task_SetCallbacks(r, ResultScreen_Update, ResultScreen_Destroy);
        if (ResultScreen_Initialize(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}

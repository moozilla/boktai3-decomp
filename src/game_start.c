#include "game_start_menu.h"
#include "game_state.h"
#include "global.h"
#include "display_sprite_playback.h"
#include "menu_parameters.h"
#include "render.h"
#include "runtime.h"
#include "ram.h"
#include "input.h"
#include "engine.h"

u8 Save_LoadSelectedSlot(void);

u8 Save_LoadAlternateSlot(void);

s32 GameStart_LoadAvailableSave(struct GameStart *unused)
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

void GameStart_RunInitHook(struct GameStart *unused)
{
}

enum SavePortrait { SAVE_PORTRAIT_SOLAR, SAVE_PORTRAIT_DARK, SAVE_PORTRAIT_VAMPIRE };

/* The save screen retains all three portrait assets and the form mapping. */

static inline u16 SavePortraitForForm(s16 form)
{
    if (form == 0)
        return SAVE_PORTRAIT_SOLAR;
    if (form == 5)
        return SAVE_PORTRAIT_DARK;
    return SAVE_PORTRAIT_VAMPIRE;
}

/* Draws the name, player portrait and title, play time, and level from a save file. */

void GameStart_DrawSaveInfo(struct GameStart *p)
{
    u8 text[0x20];
    struct GameStartSeqs seqs;
    /* Solar Boy, Dark Boy, and Vampire Boy each occupy two 16-tile strips. */
    u8 titleTiles[6][16];
    s32 i;
    const u8 *s;
    struct SceneInputResource *levelRes;

    seqs = gUnk_0824FD50;
    memcpy(titleTiles, gUnk_0824FD5C, sizeof(titleTiles));
    Bg_ClearTilemap(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    if ((s16)gGameState->flag86E != 0) {
        u8 misc[10];
        u32 zero;
        u32 *zp;
        s32 portraitIndex;
        s32 hours, minutes, level;
        u8 *q;
        struct SceneInputWidget *tens;
        struct SceneInputResource *res;
        struct SceneInputWidget *minuteOnes, *minuteTens, *colon, *levelOnes, *hourOnes;

        memcpy(misc, gUnk_0824FDBC, sizeof(misc));
        s = PlayerName_GetBuffer();
        Menu_EraseRect(8, 7, 12, 2);
        Menu_DrawText(8, 7, s);
        {
            u16 portrait = SavePortraitForForm(gGameState->form);
            u16 selected = portrait < 1 ? portrait : SAVE_PORTRAIT_SOLAR;
            portraitIndex = selected;
        }
        Anim_SetSequence(&p->cursorSequence, p->cursorResource, (u16)seqs.seq[portraitIndex], 0, 0);
        PlayerCue_StepSequenceEnded((struct ContactSwarmSprite *)(&p->cursor),
                                    (struct DisplaySequenceState *)(&p->cursorSequence));
        p->cursor.flags &= ~1;
        for (i = 0; i <= 11; i++) {
            Bg_SetTilemapEntry(3, i + 8, 6, titleTiles[portraitIndex][i], 0, 2);
            zp = &zero;
            hourOnes = &p->timeDigits[1];
            minuteOnes = &p->timeDigits[4];
            minuteTens = &p->timeDigits[3];
            colon = &p->timeDigits[2];
            levelOnes = &p->levelDigits[1];
        }
        Menu_EraseRect(3, 10, 24, 2);
        *zp = 0;
        CpuSet(zp, text, 0x05000008);
        s = Text_LookupString(Script_ParseStringRef((void *)gListMenuParameters.aBlock) +
                              SceneSequence_GetSelectedMessage());
        q = text;
        while (*s)
            *q++ = *s++;
        Menu_DrawText(3, 10, text);
        if (gGameState->lap > 0)
            p->decoration.flags &= ~1;

        tens = &p->timeDigits[0];
        hours = Div(gGameState->unk614, 0x34bc0);
        minutes = Div(gGameState->unk614, 0xe10) - hours * 60;
        if (hours > 99) {
            hours = 99;
            minutes = 59;
        }
        res = &p->itemResource;
        Anim_LoadKeyframe(hourOnes, res, (u16)Mod(hours, 10), 0);
        hours = Div(hours, 10);
        Anim_LoadKeyframe(tens, res, (u16)Mod(hours, 10), 0);
        tens->flags &= ~1;
        tens[1].flags &= ~1;
        Anim_LoadKeyframe(minuteOnes, res, (u16)Mod(minutes, 10), 0);
        minutes = Div(minutes, 10);
        Anim_LoadKeyframe(minuteTens, res, (u16)Mod(minutes, 10), 0);
        minuteTens->flags &= ~1;
        minuteOnes->flags &= ~1;
        colon->flags &= ~1;

        if (gGameState->form == 6) {
            level = gGameState->level + 10;
            if (level > 99)
                level = 99;
        } else {
            level = gGameState->level;
        }
        tens = &p->levelDigits[0];
        levelRes = &p->itemResource;
        Anim_LoadKeyframe(levelOnes, levelRes, (u16)Mod(level, 10), 0);
        level = Div(level, 10);
        Anim_LoadKeyframe(tens, levelRes, (u16)Mod(level, 10), 0);
        tens->flags &= ~1;
        tens[1].flags &= ~1;
        s = Text_LookupString(Script_ParseStringRef((void *)gListMenuParameters.mBlock) + 3);
        Menu_EraseRect(0x15, 7, 3, 2);
        Menu_DrawText(0x15, 7, s);
    } else {
        for (i = 0; i <= 8; i++)
            Bg_SetTilemapEntry(3, i + 8, 6, 1, 0, 2);
        p->cursor.flags |= 1;
        s = Text_LookupString(Script_ParseStringRef((void *)gListMenuParameters.mBlock) + 2);
        Menu_EraseRect(8, 6, 0x12, 2);
        Menu_DrawText(8, 6, s);
    }
    s = Text_LookupString(Script_ParseStringRef((void *)gListMenuParameters.mBlock) + 9);
    Menu_EraseRect(3, 0xe, 0x18, 2);
    Menu_DrawText(3, 0xe, s);
}

void GameStart_UpdateSelectionPalette(struct GameStart *p)
{
    u16 first = 0, second = 0;
    u16 *color;
    s32 base = 10;
    switch (p->selection) {
        case 0:
            color = &first;
            break;
        case 1:
            color = &second;
            break;
        default:
            return;
    }
    if (p->pulseTimer < base)
        *color = 31;
    else if (p->pulseTimer < base + 10)
        *color = 31;
    else if (p->pulseTimer < base + 18)
        *color = 27;
    else if (p->pulseTimer < base + 26)
        *color = 18;
    else if (p->pulseTimer < base + 34)
        *color = 10;
    else if (p->pulseTimer < base + 42)
        *color = 18;
    else
        *color = 27;
    if (++p->pulseTimer >= base + 50)
        p->pulseTimer = base;
    gBgPaletteBuffer[45] = first;
    gBgPaletteBuffer[44] = second;
}

// Task update: menu input (state 0), then fade out and run the chosen script (state 1).

void GameStart_Update(struct GameStart *p)
{
    s32 state;
    switch (state = p->state) {
        case 0:
            GameStart_UpdateSelectionPalette(p);
            if (gPadState[1] & 2) {
                Sound_PlaySong(0xDE);
                p->fadeSpeed = 3;
                Menu_StartFadeOut(3);
                Sound_PauseChannel10(2);
                p->state = 1;
                p->result = state;
                break;
            }
            if (gPadState[1] & 1) {
                if (p->selection == 0) {
                    Sound_PlaySong(0xE0);
                    p->fadeSpeed = 3;
                    Menu_StartFadeOut(3);
                    Sound_PauseChannel10(2);
                    p->state = 1;
                    p->result = 1;
                } else {
                    Sound_PlaySong(0xDD);
                    p->fadeSpeed = state;
                    Menu_StartFadeOut(0);
                    p->state = 1;
                    p->result = 2;
                }
                break;
            }
            if ((s16)gGameState->flag86E == 0)
                break;
            if (gPadState[1] & 0x40) {
                if (--p->selection < 0)
                    p->selection = 1;
            } else if (gPadState[1] & 0x80) {
                if (++p->selection > 1)
                    p->selection = state;
            } else
                break;
            p->pulseTimer = state;
            Sound_PlaySong(0x37C);
            break;
        case 1:
            if (p->fadeTimer > (state << p->fadeSpeed)) {
                p->cursor.flags |= state;
                p->title.flags |= state;
                p->decoration.flags |= state;
                p->background.flags |= state;
                if (p->result == 0)
                    Script_Call(gListMenuParameters.c, 0);
                else if (p->result == 2) {
                    gClockSynced = 0;
                    Script_Call(gListMenuParameters.f, 0);
                } else if (p->result == 1 && gGameState->unk5A4 != 0) {
                    gClockSynced = 0;
                    Script_Call(gListMenuParameters.l, 0);
                    gEnginePauseFlags = 0;
                    gSceneTransitionFlags = 0x11;
                    Script_SetEntryProcedure((s16)gGameState->unk5A4);
                }
                Task_Kill(p);
            }
            p->fadeTimer++;
            break;
        case 2:
            break;
    }
}

void GameStart_Destroy(struct GameStart *p)
{
    s32 i;
    struct SceneInputWidget *q;
    struct SceneInputWidget *a, *b;
    u16 *r = &gDispCntShadow;
    s32 m = 4;
    *r = *r & -m;
    q = &p->levelDigits[0];
    for (i = 1; i >= 0; i--) {
        Anim_Remove(q);
        q++;
    }
    a = &p->decoration;
    b = &p->background;
    q = &p->timeDigits[0];
    for (i = 4; i >= 0; i--) {
        Anim_Remove(q);
        q++;
    }
    Sprite_Remove(&p->cursor);
    Anim_Remove(&p->title);
    Anim_Remove(a);
    Anim_Remove(b);
}

// Tentative: sets up the Game Start screen (backgrounds, widgets, cursor sprite, save state).

s32 GameStart_Init(struct GameStart *p)
{
    u32 layers;
    struct GameStartPos pos;
    void *res;
    s32 i;
    Bg_ClearTilemap(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, 0);
    Bg_ClearTilemap(3);
    {
        void *bg = ScriptProc_Start(0xc091, 0x3536);
        layers = 4;
        Bg_SetupLayers(2, 0, bg, 0, 0, 1, &layers);
    }
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
    CpuSet(((struct GameStartPalette *)ScriptProc_Start(0x92b3, 0x204))->colors, gBgPaletteBuffer,
           0x100);
    res = ScriptProc_Start(0xcb05, 0xe2ab);
    ResourceHeader_Relocate((u8 *)&p->backgroundResource, res);
    p->itemResource = p->backgroundResource;
    Widget_InitLinked((u8 *)&p->background, (u8 *)&p->backgroundResource, 0x36, 0x10, 0, 0, 0, 0);
    ResourceHeader_Relocate((u8 *)&p->decorationResource, res);
    Widget_InitLinked((u8 *)&p->decoration, (u8 *)&p->decorationResource, 0x28, 0x10, 0, 0, 0, 0);
    p->decoration.flags |= 1;
    p->decoration.position.x = 0;
    {
        s16 *targetY = &p->decoration.position.y;
        s32 k1 = 0xffe0;
        *targetY = k1;
    }
    {
        u32 group = 0xcb05;
        res = ScriptProc_Start(group, 0x530d);
    }
    ResourceHeader_Relocate((u8 *)&p->titleResource, res);
    Widget_InitLinked((u8 *)&p->title, (u8 *)&p->titleResource, 0x8a, 0x10, 0, 0, 0, 0);
    p->title.position.x = 0;
    {
        s16 *targetY = &p->title.position.y;
        s32 valueY = 0xfff8;
        *targetY = valueY;
    }
    AnimData_Load(&p->cursorGraphics, 0x593a);
    Sprite_Init((u8 *)&p->cursor, &p->cursorGraphics, 0x11);
    p->cursor.position.x = 0x18;
    p->cursor.position.y = 0x2c;
    p->cursorResource = ScriptProc_Start(0x922e, 0x4bbf);
    Anim_SetSequence((u8 *)&p->cursorSequence, p->cursorResource, 0, 0, 0);
    PlayerCue_StepSequenceEnded(&p->cursor, &p->cursorSequence);
    pos.x = 0xa8;
    pos.y = 0x28;
    for (i = 0; i <= 4; i++) {
        Widget_InitLinked((u8 *)&p->timeDigits[i], (u8 *)&p->itemResource, (i == 2) ? (0x14) : (0),
                          0x30, 1, 0, 0x3c, &pos);
        p->timeDigits[i].position.x += i * 8;
        p->timeDigits[i].flags |= 1;
    }

    pos.x = 0xc0;
    pos.y = 0x38;
    for (i = 0; i <= 1; i++) {
        Widget_InitLinked((u8 *)&p->levelDigits[i], (u8 *)&p->itemResource, 0, 0x30, 1, 0, 0x3c,
                          &pos);
        p->levelDigits[i].position.x += i * 8;
        p->levelDigits[i].flags |= 1;
    }

    p->fadeSpeed = 0;
    if (gListMenuParameters.b == 1)
        Menu_StartFadeIn(0);
    else
        Menu_StartFadeIn(3);
    if (gListMenuParameters.b == 0) {
        u8 *name = gAnimPanelSavedSelections;
        u8 *slot = gNameEntrySavedState;
        u8 *end = name;
        s32 blank = 0x41;
        u8 *q = name + 0x17;
        do {
            *q = blank;
            q--;
        } while (((s32)q) >= ((s32)end));
        name[0x18] = 0;
        slot[0] = 4;
        slot[1] = 0;
        if (gListMenuParameters.b == 0) {
            p->saveIndex = GameStart_LoadAvailableSave(p);
            Sound_PlaySong(0xb3);
            goto check;
        }
    }
    if ((s16)gGameState->flag86E != 0)
        GameState_RestoreActiveRegions();
check:
    if ((s16)gGameState->flag86E == 0)
        p->selection = 1;

    GameStart_DrawSaveInfo(p);
    GameStart_RunInitHook(p);
    return 0;
}

/* Parameters at 0x020003A0 (meaning unknown). */

void GameStart_SetParametersFromScript(void)
{
    if (Script_SeekToKeyword('m'))
        gListMenuParameters.mBlock = Script_GetPc();
    if (Script_SeekToKeyword('a'))
        gListMenuParameters.aBlock = Script_GetPc();
    if (Script_SeekToKeyword('l'))
        gListMenuParameters.l = Script_GetValue();
    if (Script_SeekToKeyword('f'))
        gListMenuParameters.f = Script_GetValue();
    if (Script_SeekToKeyword('F'))
        gListMenuParameters.F = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        gListMenuParameters.c = Script_GetValue();
    if (Script_SeekToKeyword('b'))
        gListMenuParameters.b = Script_GetValue();
}

void *GameStart_Create(void)
{
    void *r = Task_Create(0xb, sizeof(struct GameStart));
    if (r != 0) {
        Task_SetCallbacks(r, GameStart_Update, GameStart_Destroy);
        if (GameStart_Init(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}

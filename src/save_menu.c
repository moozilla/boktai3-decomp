#include "save_menu.h"
#include "area_effect_link.h"
#include "input.h"
#include "scrolling_dialogue.h"

void TextWindow_StartPlayback(s32);

u8 Save_CopySelectedStateToAlternateSlot(void);

u8 Save_ToggleSelectedSlot(void);

void Registry1745_RequestHandshakeContinue(void);

void Registry1745_RequestHandshakeFinish(void);

void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);

void Serial_StopCommunication(void);

void System_ShutdownAndSoftReset(void);

void CharacterModel_UpdatePairedAnimations(void *, s32);

void CharacterModel_RemoveAnimations(void *);

void PaletteFade_CycleMenuColors(void *);

void LinkScorePanel_SetNumber(struct SaveMenu *, s32, s32);

void LinkScorePanel_OnWaitSessionReady(struct SaveMenu *);

void LinkScorePanel_UpdateRollingNumber(struct SaveMenu *);

void LinkScorePanel_UpdateNumberReveal(struct SaveMenu *);

void LinkScorePanel_RemoveSprites(void *);

void LinkScorePanel_LoadBackground(struct SaveMenu *);

void sub_081100A0(struct SaveMenu *);

void LinkScorePanel_InitGlyphFrames(struct SaveMenu *);

void RecordList_InitializeSprites(struct SaveMenu *);

void LinkMenu_LoadSelectionSprite(struct SaveMenu *);

void sub_0810FADC(struct SaveMenu *);

void sub_0810FDDC(struct SaveMenu *, s32, s32);

void ModifierMenu_InitSelectionSnapshot(void *, void *);

void LinkScorePanel_CreateTextWindows(struct SaveMenu *);

void LinkScorePanel_RefreshPlayerName(struct SaveMenu *);

s32 Registry1745_GetSessionState(void *);

static inline void SetUpdate(struct SaveMenu *menu, void (*update)(struct SaveMenu *), u8 state)
{
    menu->update = update;
    menu->entering = 1;
    menu->state = state;
}

static inline void SetFlag(u32 *flags, u32 mask)
{
    *flags |= mask;
}

static inline u8 FlagIsSet(u32 bits)
{
    if (bits)
        return TRUE;
    return FALSE;
}

static inline s32 GetLocalPlayer(void)
{
    struct AreaEffectLinkState *state = gLinkSession;
    if (!state)
        return -1;
    return state->localPlayer;
}

void SaveMenu_UpdateSaveDelay(struct SaveMenu *menu)
{
    if (menu->entering != 0) {
        TextWindow_SetScript(menu->windows[0], menu->statusScript);
        TextWindow_Refresh(menu->windows[0], 4);
        TextWindow_StartPlayback(menu->windows[0]);
        menu->timer = 0;
        menu->entering = 0;
    }
    if (menu->timer == 0x3c) {
        if (!(u8)Save_CopySelectedStateToAlternateSlot()) {
            SetUpdate(menu, SaveMenu_UpdateSaveFailure, 8);
        } else {
            Registry1745_RequestHandshakeContinue();
            SetUpdate(menu, SaveMenu_WaitForSaveTransition, 5);
        }
    } else {
        menu->timer++;
    }
}

void SaveMenu_WaitForSaveTransition(struct SaveMenu *menu)
{
    struct AreaEffectLinkState *g = gLinkSession;
    s32 v = -1;
    if (g != 0)
        v = g->unknown1C;
    if (v == 0x1b) {
        SetUpdate(menu, SaveMenu_UpdateSlotCommit, 6);
    }
}

void SaveMenu_UpdateSlotCommit(struct SaveMenu *menu)
{
    if (Save_ToggleSelectedSlot())
        SetUpdate(menu, SaveMenu_UpdateExitDelay, 7);
    else
        SetUpdate(menu, SaveMenu_UpdateSaveFailure, 8);
}

void SaveMenu_UpdateExitDelay(struct SaveMenu *menu)
{
    if (menu->entering) {
        TextWindow_SetScript(menu->windows[0], menu->statusScript);
        TextWindow_Refresh(menu->windows[0], 5);
        Sound_PlaySong(0x2AE);
        menu->timer = 0;
        menu->entering = 0;
    }
    if (menu->timer == 120) {
        ScreenTransition_SendCommand(3, 4, 4, 4, 4, 0xFFFF, 0);
    } else if (menu->timer > 140) {
        Registry1745_RequestHandshakeFinish();
        return;
    }
    menu->timer++;
}

void SaveMenu_UpdateSaveFailure(struct SaveMenu *menu)
{
    if (menu->entering != 0) {
        Serial_StopCommunication();
        TextWindow_SetScript(menu->windows[0], menu->statusScript);
        TextWindow_Refresh(menu->windows[0], 3);
        Sound_PlaySong(0x192);
        menu->timer = 0;
        menu->entering = 0;
    }
    if (menu->timer <= 0x3b) {
        menu->timer++;
    } else if (gPadState[1] & 0x30F) {
        System_ShutdownAndSoftReset();
    }
}

void SaveMenu_UpdateScoreTransfer(struct SaveMenu *menu)
{
    if (menu->entering) {
        menu->step = 2;
        menu->timer = 0;
        menu->entering = 0;
    }
    CharacterModel_UpdatePairedAnimations(&menu->preview.model, 0);
    PaletteFade_CycleMenuColors(&menu->paletteCycle);
    if (menu->timer > 0x3c) {
        if (menu->remaining) {
            s32 d = menu->remaining - menu->step;
            if (d < 0) {
                d += menu->step;
                gGameState->clockDisplayValue += d;
                menu->remaining -= d;
            } else {
                gGameState->clockDisplayValue += menu->step;
                menu->remaining -= menu->step;
            }
            if (gGameState->clockDisplayValue < 0)
                gGameState->clockDisplayValue = 0;
            if (gGameState->clockDisplayValue > 9999)
                gGameState->clockDisplayValue = 9999;
            if (Mod(menu->timer, 2) == 0)
                Sound_PlaySong(0x2a5);
        } else if (gPadState[1] & 0x30f) {
            Sound_PlaySong(0xdd);
            SetUpdate(menu, LinkScorePanel_OnWaitSessionReady, 3);
            return;
        }
        {
            u32 value = 1;
            u32 *flagWord = &menu->flags;
            u32 bits = *flagWord & value;
            value = (u32)flagWord;
            if (!FlagIsSet(bits) && (gPadState[1] & 0x30f)) {
                menu->step <<= 3;
                SetFlag((u32 *)value, 1);
            }
        }
    }
    LinkScorePanel_SetNumber(menu, 0, menu->remaining);
    LinkScorePanel_SetNumber(menu, 1, gGameState->clockDisplayValue);
    menu->timer++;
}

void SaveMenu_UpdateScoreReveal(struct SaveMenu *menu)
{
    u8 *redraw = &menu->entering;
    u16 *timer;
    if (*redraw) {
        Bg_SetLayerMap(3, menu->backgroundResource, 3);
        BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
        TextWindow_SetScript(menu->windows[0], menu->scoreScript);
        TextWindow_SetScript(menu->windows[1], menu->scoreScript);
        TextWindow_SetScript(menu->windows[2], menu->scoreScript);
        TextWindow_SetScript(menu->windows[3], menu->scoreScript);
        TextWindow_SetScript(menu->windows[4], menu->scoreScript);
        TextWindow_SetScript(menu->windows[5], menu->scoreScript);
        TextWindow_SetScript(menu->windows[6], menu->scoreScript);
        TextWindow_Refresh(menu->windows[0], 0);
        TextWindow_Refresh(menu->windows[1], 1);
        TextWindow_Refresh(menu->windows[2], 2);
        TextWindow_Refresh(menu->windows[3], 3);
        TextWindow_Refresh(menu->windows[4], 4);
        TextWindow_Refresh(menu->windows[5], 5);
        TextWindow_Refresh(menu->windows[6], 6);
        TextWindow_StartPlayback(menu->windows[0]);
        TextWindow_StartPlayback(menu->windows[1]);
        TextWindow_StartPlayback(menu->windows[2]);
        TextWindow_StartPlayback(menu->windows[3]);
        TextWindow_StartPlayback(menu->windows[4]);
        TextWindow_StartPlayback(menu->windows[5]);
        TextWindow_StartPlayback(menu->windows[6]);
        menu->timer = 0;
        *redraw = 0;
    }
    timer = &menu->timer;
    if (*timer > 9) {
        CharacterModel_UpdatePairedAnimations(&menu->preview.model, 0);
        PaletteFade_CycleMenuColors(&menu->paletteCycle);
        if (!menu->skip) {
            SetUpdate(menu, SaveMenu_UpdateScoreTransfer, 1);
        } else {
            if (gPadState[1] & 0x30b)
                menu->skip = 1;
            LinkScorePanel_UpdateRollingNumber(menu);
            LinkScorePanel_UpdateNumberReveal(menu);
            LinkScorePanel_SetNumber(menu, 1, gGameState->clockDisplayValue);
            (*timer)++;
        }
    } else {
        (*timer)++;
    }
}

s32 SaveMenu_Update(struct SaveMenu *menu)
{
    if (Registry1745_GetSessionState(menu) == 0) {
        ScrollingDialogue_HideImmediately();
        Task_Kill(menu);
        return -1;
    }
    if (menu->update != 0)
        menu->update(menu);
    return 1;
}

u32 SaveMenu_Destroy(struct SaveMenu *menu)
{
    LinkScorePanel_RemoveSprites(menu);
    CharacterModel_RemoveAnimations(&menu->preview.model);
    gSaveMenu = 0;
    return 1;
}

s32 SaveMenu_InitializeFromScript(struct SaveMenu *menu)
{
    if (Script_SeekToKeyword('s')) {
        if (!(menu->titleScript = Script_GetTextSourcePc()))
            return -1;
    }
    if (Script_SeekToKeyword('S')) {
        if (!(menu->statusScript = Script_GetTextSourcePc()))
            return -1;
    }
    if (Script_SeekToKeyword('n')) {
        if (!(menu->nameScript = Script_GetPc()))
            return -1;
    }
    if (Script_SeekToKeyword('F')) {
        if (!(menu->scoreScript = Script_GetTextSourcePc()))
            return -1;
    }
    menu->localPlayer = GetLocalPlayer();
    menu->exitScript = Script_GetKeywordValue('e', 0);
    Bg_ClearTilemap(3);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(0);
    LinkScorePanel_LoadBackground(menu);
    sub_081100A0(menu);
    LinkScorePanel_InitGlyphFrames(menu);
    RecordList_InitializeSprites(menu);
    LinkMenu_LoadSelectionSprite(menu);
    sub_0810FADC(menu);
    sub_0810FDDC(menu, 0, gGameState->linkScoreFirstRow);
    sub_0810FDDC(menu, 1, gGameState->linkScoreOtherRows[0]);
    sub_0810FDDC(menu, 2, gGameState->linkScoreOtherRows[1]);
    ModifierMenu_InitSelectionSnapshot(&menu->paletteCycle, menu->paletteResource);
    LinkScorePanel_CreateTextWindows(menu);
    LinkScorePanel_RefreshPlayerName(menu);
    {
        void (*cb)(struct SaveMenu *) = SaveMenu_UpdateScoreReveal;
        u8 nine = 9;
        menu->update = cb;
        menu->entering = 1;
        menu->state = nine;
    }
    if (Script_GetKeywordValue('f', 0))
        ScreenTransition_SendCommand(2, 5, 4, 4, 4, 0xffff, 0);
    gSaveMenu = menu;
    return 1;
}

struct SaveMenu *SaveMenu_CreateFromScript(void)
{
    struct SaveMenu *menu;
    if (gSaveMenu != 0)
        return (void *)gSaveMenu;
    menu = Task_Create(0xb, 0x1034);
    if (menu != 0) {
        Task_SetCallbacks(menu, (void (*)(void *))SaveMenu_Update,
                          (void (*)(void *))SaveMenu_Destroy);
        if (SaveMenu_InitializeFromScript(menu) < 0) {
            Task_Kill(menu);
            return 0;
        }
    }
    return menu;
}

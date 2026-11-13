#include "scene_dialog.h"

void PauseMenu_CloseTextWindows(void *);
void MessageWindow_Clear(void);
void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_RequestHide(void);
void TextWindow_Clear(u32);
void MessageWindow_SetBounds(s32, s32, s32, s32);
void Hud_SetupBackgroundLayer(void *, u32, s32, s32, u32);
void PauseMenu_HideAllSprites(void *);
void PauseMenu_LoadSpriteKeyframe(void *, s32, s32);
void MessageWindow_SetMessageTable(const void *);
void MessageWindow_SetPalette(s32);
void PauseMenu_SetPromptIconPositions(void *, s32, s32);
void SceneHud_SetPairedIcons(void *, s32, s32);
void Hud_SetInteractionCallback(void *, void (*)(struct SceneHudState *), s32);
void PauseMenu_SetConfirmationIndicator(void *, s32);
void PauseMenu_HidePromptIcons(void *);
void MenuDisplay_SetPaletteOverrides(void *, int, int, int, int);
int SceneHud_UpdateBinaryChoice(void *);
void TextWindow_SetBounds(s32, s32, s32, s32, s32);
void TextWindow_StartPlayback(int);
u8 Save_WriteAlternateSlotAndSelect(void);
void SceneHud_DrawSelectedClock(void *);
void PauseMenu_SetSpritePosition(void *, s32, s32, s32);
void PauseMenu_ClearSpriteFlags(void *, s32, u32);
void sub_08215AC4(u32, u32);
void Mosaic_ResetParameters(void);
void Hud_RefreshActivePanel(void *);
void MusicSlots_StopAllAndRetainResumable(void);
void Hud_InitSelectionBounds(u32, u32);
void Hud_SetUpdateCallback(void *, void (*)(struct SceneHudState *));
void PauseMenu_UpdateHighlightPalette(void *);
void SceneHud_FadeOutPage(void *);
extern void *gUnk_08611C34[];
void PauseMenu_InitSleepTab(struct SceneHudState *menu)
{
    PauseMenu_CloseTextWindows(menu);
    MessageWindow_Clear();
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_RequestHide();
    TextWindow_Clear(menu->catalogTextWindow);
    TextWindow_Clear(menu->timedMessageWindow);
    MessageWindow_SetBounds(1, 0x10, 0x1D, 2);
    Hud_SetupBackgroundLayer(menu, menu->panelResource, 0, 11, menu->panelConfig);
    PauseMenu_HideAllSprites(menu);
    PauseMenu_LoadSpriteKeyframe(menu, 0, 0x59);
    MessageWindow_SetMessageTable(menu->optionUpperEScript);
    MessageWindow_SetPalette(0xEFDA);
    SceneCatalog_SelectDescription(0x11);
    menu->pageInputCooldown = 0;
    PauseMenu_SetPromptIconPositions(menu, 0, -12);
    SceneHud_SetPairedIcons(menu, 1, -1);
    Hud_SetInteractionCallback(menu, PauseMenu_BeginSleepPrompt, 0);
}

void SelectionPanel_ConfirmTransition(struct SceneHudState *menu)
{
    if (gPadState[1] & 1) {
        Sound_PlaySong(0xDD);
        PauseMenu_SetConfirmationIndicator(menu, 1);
        SceneCatalog_SelectDescription(9);
        PauseMenu_HidePromptIcons(menu);
        Hud_SetInteractionCallback(menu, SelectionPanel_ProcessFlaggedChoice, 1);
        MenuDisplay_SetPaletteOverrides(menu, 0, 0, 1, 0);
    }
}

void SelectionPanel_ProcessFlaggedChoice(struct SceneHudState *menu)
{
    int choice = SceneHud_UpdateBinaryChoice(menu);
    if (choice == 0) {
        Sound_PlaySong(0xDE);
        SceneCatalog_SelectDescription(8);
        SceneHud_SetPairedIcons(menu, 1, -1);
        Hud_SetInteractionCallback(menu, SelectionPanel_ConfirmTransition, 0);
    } else if (choice == 1) {
        Sound_PlaySong(0xDD);
        if ((s16)gGameState->flag86E == 0) {
            Hud_SetInteractionCallback(menu, SceneDialog_BeginTimedMessage, 1);
        } else {
            PauseMenu_SetConfirmationIndicator(menu, 0);
            SceneCatalog_SelectDescription(10);
            Hud_SetInteractionCallback(menu, SelectionPanel_ProcessSecondChoice, 1);
        }
    }
}

void SelectionPanel_ProcessSecondChoice(struct SceneHudState *menu)
{
    int choice = SceneHud_UpdateBinaryChoice(menu);
    if (choice == 0) {
        Sound_PlaySong(0xDE);
        SceneCatalog_SelectDescription(8);
        SceneHud_SetPairedIcons(menu, 1, -1);
        Hud_SetInteractionCallback(menu, SelectionPanel_ConfirmTransition, 0);
    } else if (choice == 1) {
        Sound_PlaySong(0xDD);
        Hud_SetInteractionCallback(menu, SceneDialog_BeginTimedMessage, 1);
    }
}

void SceneDialog_BeginTimedMessage(struct SceneHudState *menu)
{
    gSceneDialogActive = 1;
    if (!menu->actionTransferDelay) {
        TextWindow_SetBounds(menu->timedMessageWindow, 2, 6, 26, 6);
        TextWindow_SetScript(menu->timedMessageWindow, menu->optionUpperEScript);
        TextWindow_Refresh(menu->timedMessageWindow, 13);
        TextWindow_StartPlayback(menu->timedMessageWindow);
        SceneCatalog_SelectDescription(12);
    }
    if (++menu->actionTransferDelay > 59)
        Hud_SetInteractionCallback(menu, SceneDialog_SelectResultPhase, 1);
}

void SceneDialog_SelectResultPhase(struct SceneHudState *menu)
{
    gSceneDialogActive = 1;
    if (Save_WriteAlternateSlotAndSelect()) {
        gGameState->flag86E = 1;
        Hud_SetInteractionCallback(menu, SceneDialog_FinishSuccessfulResult, 1);
    } else
        Hud_SetInteractionCallback(menu, SceneDialog_FinishFailedResult, 1);
}

void SceneDialog_FinishSuccessfulResult(struct SceneHudState *menu)
{
    if (!menu->actionTransferDelay) {
        Sound_PlaySong(0x2AE);
        TextWindow_Clear(menu->timedMessageWindow);
        SceneCatalog_SelectDescription(14);
    }
    if (++menu->actionTransferDelay > 119) {
        SceneCatalog_SelectDescription(8);
        SceneHud_SetPairedIcons(menu, 1, -1);
        Hud_SetInteractionCallback(menu, SelectionPanel_ConfirmTransition, 0);
    }
}

void SceneDialog_FinishFailedResult(struct SceneHudState *menu)
{
    if (!menu->actionTransferDelay) {
        Sound_PlaySong(0x192);
        TextWindow_Clear(menu->timedMessageWindow);
        SceneCatalog_SelectDescription(15);
    }
    if (++menu->actionTransferDelay > 119) {
        SceneCatalog_SelectDescription(8);
        SceneHud_SetPairedIcons(menu, 1, -1);
        Hud_SetInteractionCallback(menu, SelectionPanel_ConfirmTransition, 0);
    }
}

void PauseMenu_EnterPrompt(struct SceneHudState *menu)
{
    PauseMenu_CloseTextWindows(menu);
    MessageWindow_Clear();
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_RequestHide();
    TextWindow_Clear(menu->catalogTextWindow);
    TextWindow_Clear(menu->timedMessageWindow);
    MessageWindow_SetBounds(1, 0x10, 0x1D, 2);
    Hud_SetupBackgroundLayer(menu, menu->panelResource, 0, 0xB, menu->panelConfig);
    PauseMenu_HideAllSprites(menu);
    PauseMenu_LoadSpriteKeyframe(menu, 0, 0x5A);
    MessageWindow_SetMessageTable(menu->optionUpperEScript);
    MessageWindow_SetPalette(0xEFDA);
    SceneCatalog_SelectDescription(8);
    PauseMenu_SetPromptIconPositions(menu, 0, -12);
    SceneHud_SetPairedIcons(menu, 1, -1);
    Hud_SetInteractionCallback(menu, SelectionPanel_ConfirmTransition, 0);
}

void SceneDialog_ConfigureVariantSlot(struct SceneHudState *menu)
{
    Callback_InvokeActor(menu, gUnk_08611C34[menu->mode]);
    SceneHud_DrawSelectedClock(menu);
    PauseMenu_SetSpritePosition(menu, 3, menu->mode * 8, 0);
    PauseMenu_ClearSpriteFlags(menu, 3, 1);
}

void PauseMenu_SaveDisplayState(struct SceneHudState *menu)
{
    u32 flags;
    u32 bgEnable;
    menu->initializationState = 1;
    gRenderListIndex = 1;
    flags = gDispCntShadow;
    menu->savedDisplayControl = flags;
    bgEnable = 0x100;
    flags &= ~bgEnable;
    flags &= ~0x200;
    flags &= ~0x400;
    flags &= ~0x800;
    gDispCntShadow = flags;
    CpuSet(Video_GetBackgroundMap(1), menu->savedBackgroundMap, 0x04000200);
    menu->savedBackground = gBgRecords[1];
    menu->savedBlendControl = *(vu16 *)0x04000050;
    menu->savedBackgroundControl = *(vu16 *)0x0400000A;
    *(vu16 *)0x04000050 = 0;
    gFadeMultiplier = 0x40;
    sub_08215AC4(1, 2);
    Bg_SetPriority(1, 1);
    menu->savedMosaicHalfword = gMosaicHalfword;
    menu->savedMosaicParameter = gMosaicParam;
    menu->savedSpriteMosaicAttribute = gSpriteMosaicAttr;
    menu->savedMosaicByte = gMosaicByte;
    Mosaic_ResetParameters();
    gDispCntShadow |= bgEnable;
    menu->statusRefreshRequested = 0;
    SceneDialog_ConfigureVariantSlot(menu);
    menu->allocationDescription = 0xFF;
}

void PauseMenu_RestoreDisplayState(struct SceneHudState *menu)
{
    u32 bgEnable;
    u32 flags;
    void *map;
    menu->initializationState = 0;
    gRenderListIndex = 0;
    bgEnable = 0x100;
    flags = gDispCntShadow;
    flags &= ~bgEnable;
    flags &= ~0x200;
    gDispCntShadow = flags;
    PauseMenu_HideAllSprites(menu);
    sub_08215AC4(1, 0);
    map = Video_GetBackgroundMap(1);
    CpuSet(menu->savedBackgroundMap, map, 0x04000200);
    gBgRecords[1] = menu->savedBackground;
    *(vu16 *)0x0400000A = menu->savedBackgroundControl;
    gMosaicHalfword = 1;
    gMosaicParam = menu->savedMosaicParameter;
    gSpriteMosaicAttr = menu->savedSpriteMosaicAttribute;
    gMosaicByte = menu->savedMosaicByte;
    MessageWindow_Clear();
    ScrollingDialogue_RequestHide();
    PauseMenu_CloseTextWindows(menu);
    gDispCntShadow |= bgEnable;
    Hud_SetupBackgroundLayer(menu, menu->panelResource, 0, 0, menu->panelConfig);
    menu->pageTransitions[0] = 0;
    menu->pageTransitions[1] = 0;
    Hud_RefreshActivePanel(menu);
}

s32 SceneDialog_FindNextEnabledEntry(struct SceneHudState *menu, s32 current)
{
    s32 next = current + 1;
    s32 i;
    if (next > 9)
        next = 0;
    for (i = 0; i <= 9; i++) {
        if (menu->pageEnabled[next])
            return next;
        next++;
        if (next > 9)
            next = 0;
    }
    return current;
}

s32 SceneDialog_FindPreviousEnabledEntry(struct SceneHudState *menu, s32 current)
{
    s32 previous = current - 1;
    s32 i;
    if (current == 0)
        previous = 9;
    for (i = 0; i <= 9; i++) {
        if (menu->pageEnabled[previous])
            return previous;
        if (previous == 0)
            previous = 9;
        else
            previous--;
    }
    return current;
}

void SceneDialog_UpdateEntryFade(struct SceneHudState *menu)
{
    if (menu->pageFadeTimer == 1)
        MusicSlots_StopAllAndRetainResumable();
    if (menu->pageFadeTimer == 2)
        Sound_PlaySong(0x118);
    if (menu->pageFadeTimer <= 15)
        Hud_InitSelectionBounds(64 - menu->pageFadeTimer * 4, 0x1084);
    else
        Hud_InitSelectionBounds(0, 0x1084);
    if (++menu->pageFadeTimer > 18) {
        PauseMenu_SaveDisplayState(menu);
        Hud_SetUpdateCallback(menu, Hud_UpdateBlendRamp);
    }
}

void Hud_UpdateBlendRamp(struct SceneHudState *menu)
{
    SceneHud_DrawSelectedClock(menu);
    Hud_InitSelectionBounds(menu->pageFadeTimer * 4, 0x1084);
    if (++menu->pageFadeTimer > 16) {
        menu->comparisonModifier = 0;
        Hud_SetUpdateCallback(menu, SceneHud_UpdatePageInput);
    }
}

void SceneHud_UpdatePageInput(struct SceneHudState *menu)
{
    u32 direction;
    Hud_InitSelectionBounds(0x40, 0x1084);
    SceneHud_DrawSelectedClock(menu);
    PauseMenu_UpdateHighlightPalette(menu);
    if (menu->statusRefreshRequested)
        goto close;
    if (menu->comparisonModifier) {
        PauseMenu_LoadSpriteKeyframe(menu, 1, 0x43);
        PauseMenu_LoadSpriteKeyframe(menu, 2, 0x44);
        Callback_InvokeActor(menu, menu->pageCallback);
        return;
    }
    PauseMenu_LoadSpriteKeyframe(menu, 1, 0x41);
    PauseMenu_LoadSpriteKeyframe(menu, 2, 0x42);
    if (gPadState[1] & 2) {
        Sound_PlaySong(0x119);
    close:
        Hud_SetUpdateCallback(menu, SceneHud_FadeOutPage);
        return;
    }
    if (!menu->pageInputCooldown) {
        direction = 0;
        if (gPadState[1] & 0x100) {
            menu->pageRepeatTimer = 0;
            direction = 1;
        } else if (gPadState[1] & 0x200) {
            menu->pageRepeatTimer = 0;
            direction = 2;
        } else if (gPadState[0] & 0x100) {
            if (++menu->pageRepeatTimer > 9) {
                menu->pageRepeatTimer = 0;
                direction = 1;
            }
        } else if (gPadState[0] & 0x200) {
            if (++menu->pageRepeatTimer > 9) {
                menu->pageRepeatTimer = 0;
                direction = 2;
            }
        }
        if (direction == 1) {
            Sound_PlaySong(0x10A);
            menu->mode = SceneDialog_FindNextEnabledEntry(menu, menu->mode);
        } else if (direction == 2) {
            Sound_PlaySong(0x10A);
            menu->mode = SceneDialog_FindPreviousEnabledEntry(menu, menu->mode);
        } else
            goto callback;
        SceneDialog_ConfigureVariantSlot(menu);
        return;
    } else
        --menu->pageInputCooldown;
callback:
    Callback_InvokeActor(menu, menu->pageCallback);
}

#include "scene_catalog.h"

s32 MessageWindow_SetDisplayMode(s32);
s32 MessageWindow_Clear(void);
s32 MessageWindow_SetMessageTable(const void *);
s32 MessageWindow_ShowMessage(s32);
s32 MessageWindow_SetBounds(s32, s32, s32, s32);
void Hud_DrawHealthRatio(void *, s32, s32, s32);
void Hud_DrawEnergyRatio(void *, s32, s32, s32);
void PauseMenu_HidePromptIcons(void *);
void Hud_DrawHealthMeter(void *, s32, s32, s32, s32);
void Hud_DrawEnergyMeter(void *, s32, s32, s32, s32, s32, s32);
void PauseMenu_SetPairedIcons(void *, s32, s32);
u32 PauseMenu_ReadDirectionRepeat(void *);
void Presentation_UpdateListCursor(void *);
void Presentation_DrawEntryCount(void *);
void InventoryGrid_LayoutStatMarkers(void *);
void PauseMenu_SetConfirmationIndicator(void *, u32);
void MenuDisplay_SetPaletteOverrides(void *, u32, u32, u32, u32);
void Hud_SetInteractionCallback(struct SceneHudState *, void (*)(struct SceneHudState *), u32);
void Presentation_InitListValues(void *);
void PauseMenu_SetPromptIconPositions(void *, u32, u32);
void InventoryGrid_UpdateStatPreview(void *);
int PauseMenu_UpdateBinaryChoice(void *);
void Presentation_CommitListValues(void *);
void Player_RefreshStatsAndVitals(void *);
void Hud_DrawDefaultHealthMeter(void *, u32, u32, u32);
void Hud_DrawDefaultEnergyMeter(void *, u32, u32, u32, u32, u32);
u16 *Hud_GetTilemapCell(u32, u32, u32);
s32 Hud_GetMinimum(s32, s32);
void Hud_DrawDecimalDigits(s32, s32, s32, s32, s32);
void Hud_FillTileRun(s32, s32, s32);
void TextWindow_StartPlayback(s32);
u32 SunClock_GetPhase(void);
void Hud_DrawClockLabelA(u32, u32);
void Hud_DrawSunClockTimeA(u32, u32);
void Hud_DrawClockLabelB(u32, u32);
void Hud_DrawSunClockTimeB(u32, u32);
void PauseMenu_CloseTextWindows(void *);
void Hud_SetupBackgroundLayer(void *, u32, u32, u32, u32);
void PauseMenu_HideAllSprites(void *);
void PauseMenu_LoadSpriteKeyframe(void *, u32, u32);
void PauseMenu_ClearSpriteFlags(void *, u32, u32);
void PauseMenu_ShowPromptSprite(void *, u32, u32);
void Hud_DrawHeaderCounter(void);
void InventoryPanel_UpdateSelectedUnlock(void *, u32, u32, u32, u32);
void InventoryPanel_UpdateSelectedEquipment(void *, u32, u32, u32, u32);
void MenuDisplay_SnapshotValues(void *);
void InventoryPanel_ShowStatusIcons(void *);
void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_RequestHide(void);
void TextWindow_Clear(u32);
void Hud_DrawCounter7B0(u32, u32);
void Hud_DrawCounter50C(u32, u32);
void Presentation_SetAmountGlyphRows(void *);
void GameState_BackupRegionRange(void *, u32);

static inline void SceneCatalog_QueueDescription(struct SceneHudState *state, u32 message)
{
    state->allocationDescription = message;
}

void SceneCatalog_SetDescription(struct SceneHudState *s, s32 selection)
{
    if (selection < 0) {
        MessageWindow_Clear();
    } else {
        MessageWindow_SetDisplayMode(1);
        MessageWindow_SetMessageTable(s->optionUpperEScript);
        MessageWindow_SetBounds(0, 18, 30, 2);
        MessageWindow_ShowMessage(selection);
    }
}

void InventoryGrid_UpdateModifierPreview(struct SceneHudState *p)
{
    u16 key = gPadState[0] & 4;
    if (key) {
        p->comparisonModifier = 1;
        Hud_DrawHealthRatio(p, 1, 7, 8);
        Hud_DrawEnergyRatio(p, 1, 7, 10);
        PauseMenu_HidePromptIcons(p);
    } else {
        p->comparisonModifier = key;
        Hud_DrawHealthMeter(p, 1, 8, 8, 1);
        Hud_DrawEnergyMeter(p, 1, 8, 10, p->actionOwner->currentStatB, p->actionOwner->maximumStatB, 1);
        PauseMenu_SetPairedIcons(p, 1, -1);
    }
    p->comparisonIndicators[0].flags |= 1;
    p->comparisonIndicators[1].flags |= 1;
}

static inline void SceneCatalog_ShowWidget(struct SceneInputWidget *widget)
{
    widget->flags &= ~1;
}

void Presentation_UpdateAllocationSelection(struct SceneHudState *menu)
{
    s32 inputState;
    s32 statIndex;
    s16 *adjustments;
    u32 key;
    menu->comparisonModifier = 0;
    inputState = 0;
    adjustments = menu->comparisonOffsets;
    for (statIndex = 2; statIndex >= 0; statIndex--) {
        if (*adjustments > 0) {
            inputState = 1;
            menu->comparisonModifier = inputState;
        }
        adjustments++;
    }
    if (!inputState && (gPadState[0] & 4)) {
        u8 *modifier = &menu->comparisonModifier;
        inputState = 1;
        *modifier = inputState;
        Hud_DrawHealthRatio(menu, 1, 7, 8);
        Hud_DrawEnergyRatio(menu, 1, 7, 10);
        menu->comparisonIndicators[0].flags |= inputState;
        menu->comparisonIndicators[1].flags |= inputState;
        menu->categoryWidgets[5].flags |= inputState;
        menu->motionWidgets[0].flags |= inputState;
        PauseMenu_HidePromptIcons(menu);
        goto refreshPreview;
    }
    key = PauseMenu_ReadDirectionRepeat(menu);
    if (key & 0x40) {
        if (!menu->rowIndex) {
            menu->rowIndex = 2;
        } else {
            menu->rowIndex--;
        }
        Presentation_UpdateListCursor(menu);
        Sound_PlaySong(0xdc);
        SceneCatalog_QueueDescription(menu, menu->rowIndex + 20);
    } else if (key & 0x80) {
        if (menu->rowIndex > 1) {
            menu->rowIndex = 0;
        } else {
            menu->rowIndex++;
        }
        Presentation_UpdateListCursor(menu);
        Sound_PlaySong(0xdc);
        SceneCatalog_QueueDescription(menu, menu->rowIndex + 20);
    } else if (key & 0x20) {
        u32 offset = menu->rowIndex << 1;
        u8 *base;
        s16 *adjustment;
        base = (u8 *)menu->comparisonOffsets;
        adjustment = (s16 *)(base + offset);
        if (*adjustment > 0) {
            (*adjustment)--;
            (menu->entryCount)++;
            goto allocationChanged;
        }
    } else if (({
                   inputState = 0x10;
                   inputState &= key;
                   inputState;
               })) {
        u16 *remaining = &menu->entryCount;
        s32 index;
        s32 stat;
        s16 *adjustment;
        struct Player *player;
        if (!(*remaining)) {
            goto refreshDisplay;
        }
        player = menu->actionOwner;
        index = menu->rowIndex;
        {
            u32 offset = index << 1;
            u8 *base;
            u8 *stats = (u8 *)player->derivedStats;
            stat = *(u16 *)(stats + offset);
            base = (u8 *)menu->comparisonOffsets;
            adjustment = (s16 *)(base + offset);
        }
        if ((stat + (*adjustment)) > 99) {
            goto refreshDisplay;
        }
        if ((menu->comparisonValues[index] + (*adjustment)) > 99) {
            goto refreshDisplay;
        }
        (*adjustment)++;
        (*remaining)--;
    allocationChanged:
        Presentation_DrawEntryCount(menu);
        InventoryGrid_LayoutStatMarkers(menu);
        Sound_PlaySong(0x19a);
    } else {
        if ((gPadState[1] & 1) && menu->comparisonModifier) {
            Sound_PlaySong(0xdd);
            menu->allocationDescription = 24;
            PauseMenu_SetConfirmationIndicator(menu, 0);
            MenuDisplay_SetPaletteOverrides(menu, 0, 0, 1, inputState);
            Hud_SetInteractionCallback(menu, MenuSelection_ProcessConfirmation, 1);
        } else if (gPadState[1] & 2) {
            Sound_PlaySong(0xde);
            Presentation_InitListValues(menu);
            Presentation_DrawEntryCount(menu);
            InventoryGrid_LayoutStatMarkers(menu);
        }
    }
    do {
    refreshDisplay:
        menu->comparisonIndicators[0].flags &= ~1;
        if (menu->comparisonModifier) {
            SceneCatalog_ShowWidget(&menu->comparisonIndicators[1]);
            menu->comparisonIndicators[1].position.x = 0;
            menu->comparisonIndicators[1].position.y = 8;
            PauseMenu_HidePromptIcons(menu);
            PauseMenu_SetPromptIconPositions(menu, 0, 8);
            PauseMenu_SetPairedIcons(menu, 1, -1);
        } else {
            menu->comparisonIndicators[1].flags |= 1;
            PauseMenu_SetPromptIconPositions(menu, 0, 8);
            PauseMenu_SetPairedIcons(menu, 1, -1);
        }
        menu->categoryWidgets[5].flags &= ~1;
        Anim_Step(&menu->categoryWidgets[5], menu->widgetResource);
    refreshPreview:
        InventoryGrid_UpdateStatPreview(menu);
        if (menu->allocationDescription >= 0) {
            SceneCatalog_SetDescription(menu, menu->allocationDescription);
            menu->allocationDescription = 255;
        }
    } while (0);
}

void MenuSelection_ProcessConfirmation(struct SceneHudState *s)
{
    s32 state = PauseMenu_UpdateBinaryChoice(s);
    if (state == 0) {
        Sound_PlaySong(0xDE);
        SceneCatalog_SetDescription(s, s->rowIndex + 20);
        PauseMenu_SetPairedIcons(s, 1, -1);
        Hud_SetInteractionCallback(s, Presentation_UpdateAllocationSelection, 1);
        MenuDisplay_SetPaletteOverrides(s, 1, 0, 0, 0);
    } else if (state == 1) {
        struct Player **p;
        s32 neg;
        u32 count;
        Sound_PlaySong(0xDD);
        Presentation_CommitListValues(s);
        p = &s->actionOwner;
        Player_RefreshStatsAndVitals(*p);
        Presentation_DrawEntryCount(s);
        InventoryGrid_LayoutStatMarkers(s);
        Hud_DrawDefaultHealthMeter(s, 1, 8, 8);
        Hud_DrawDefaultEnergyMeter(s, 1, 8, 10, (*p)->currentStatB, (*p)->maximumStatB);
        neg = -1;
        PauseMenu_SetPairedIcons(s, 1, neg);
        count = s->entryCount;
        if (count == 0) {
            s->categoryWidgets[5].flags |= state;
            SceneCatalog_SetDescription(s, neg);
            Hud_SetInteractionCallback(s, InventoryGrid_UpdateModifierPreview, 0);
            MenuDisplay_SetPaletteOverrides(s, 0, 0, 0, count);
        } else {
            SceneCatalog_SetDescription(s, s->rowIndex + 20);
            MenuDisplay_SetPaletteOverrides(s, 1, 0, 0, 0);
            Hud_SetInteractionCallback(s, Presentation_UpdateAllocationSelection, 0);
        }
    }
}

s32 SceneCatalog_GetLevelScaledValue(struct SceneHudState *s)
{
    const s32 *values = (const s32 *)s->optionEValue;
    values += gGameState->level;
    return *values * 10;
}

void MenuSelection_DrawNumericSummary(struct SceneHudState *s)
{
    if (s->actionOwner->actionMode != 6) {
        s32 zero;
        s32 max;
        s32 divisor;
        s32 value;
        Hud_DrawDecimalDigits(Hud_GetMinimum(gGameState->level, 99), 10, 21, 3, 0);
        zero = 0;
        value = Hud_GetMinimum(gGameState->rewardTotal, max = 999999);
        Hud_DrawDecimalDigits(value, divisor = 100000, 23, 4, zero);
        if (gGameState->level > 98) {
            u16 *p = Hud_GetTilemapCell(0, 23, 5);
            s32 i;
            i = 0;
            do {
                *p++ = 0xF020;
            } while (++i < 6);
        } else {
            Hud_DrawDecimalDigits(Hud_GetMinimum(SceneCatalog_GetLevelScaledValue(s), max),
                                       divisor, 23, 5, zero);
        }
    } else {
        Hud_DrawDecimalDigits(Hud_GetMinimum(gGameState->level + 10, 99), 10, 21, 3, 0);
        Hud_FillTileRun(6, 23, 4);
        Hud_FillTileRun(6, 23, 5);
        Hud_FillTileRun(2, 20, 8);
        Hud_DrawDecimalDigits(99, 10, 25, 8, 0);
    }
}

void SceneCatalog_SetSelection(struct SceneHudState *s, u32 selection)
{
    struct SceneInputWidget *animation = &s->categoryWidgets[5];
    animation->flags &= ~1;
    Anim_LoadKeyframe(animation, s->widgetResource, 0x7C, 0);
    s->rowIndex = selection;
    Presentation_UpdateListCursor(s);
}

void SceneCatalog_ResetTextWindow(struct SceneHudState *s)
{
    s32 *window = &s->primaryTextWindow;
    TextWindow_SetScript(*window, s->optionUpperEScript);
    TextWindow_Refresh(*window, 0);
    TextWindow_StartPlayback(*window);
}

void Hud_DrawStatusDayNightIndicators(struct SceneHudState *s)
{
    SunClock_GetBaseHour();
    SunClock_GetBaseMinute();
    switch (SunClock_GetPhase()) {
        case 0:
        case 4:
        case 5:
            Hud_DrawClockLabelA(0x13, 0xb);
            Hud_DrawSunClockTimeA(0x18, 0xb);
            break;
        case 1:
        case 2:
        case 3:
            Hud_DrawClockLabelB(0x13, 0xb);
            Hud_DrawSunClockTimeB(0x18, 0xb);
            break;
    }
}

void Hud_InitStatusMenu(struct SceneHudState *s)
{
    struct Player *p;
    s32 neg;
    u32 zero;
    PauseMenu_CloseTextWindows(s);
    MessageWindow_Clear();
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_RequestHide();
    TextWindow_Clear(s->primaryTextWindow);
    TextWindow_Clear(s->timedMessageWindow);
    Hud_SetupBackgroundLayer(s, s->panelResource, 0, 9, s->panelConfig);
    MenuDisplay_SnapshotValues(s);
    Presentation_InitListValues(s);
    MenuSelection_DrawNumericSummary(s);
    Hud_DrawDefaultHealthMeter(s, 1, 8, 8);
    p = s->actionOwner;
    Hud_DrawDefaultEnergyMeter(s, 1, 8, 10, p->currentStatB, p->maximumStatB);
    Hud_DrawHeaderCounter();
    Hud_DrawCounter7B0(25, 9);
    Hud_DrawCounter50C(25, 10);
    Hud_DrawStatusDayNightIndicators(s);
    PauseMenu_HideAllSprites(s);
    PauseMenu_LoadSpriteKeyframe(s, 0, 0x57);
    PauseMenu_ClearSpriteFlags(s, 0x4D, 1);
    PauseMenu_ClearSpriteFlags(s, 0x4E, 1);
    PauseMenu_ShowPromptSprite(s, 8, 0x20);
    Presentation_SetAmountGlyphRows(s);
    SceneCatalog_SetSelection(s, 0);
    InventoryGrid_LayoutStatMarkers(s);
    InventoryPanel_ShowStatusIcons(s);
    SceneCatalog_ResetTextWindow(s);
    PauseMenu_SetPromptIconPositions(s, 0, 8);
    neg = -1;
    PauseMenu_SetPairedIcons(s, 1, neg);
    zero = 0;
    InventoryPanel_UpdateSelectedUnlock(s, 20, 7, 0, zero);
    InventoryPanel_UpdateSelectedEquipment(s, 26, 7, 0, zero);
    if (!s->entryCount) {
        s->categoryWidgets[5].flags |= 1;
        s->allocationDescription = 0xFF;
        SceneCatalog_SetDescription(s, neg);
        Hud_SetInteractionCallback(s, InventoryGrid_UpdateModifierPreview, 0);
        MenuDisplay_SetPaletteOverrides(s, 0, 0, 0, zero);
    } else {
        struct SceneInputWidget *obj = &s->comparisonIndicators[0];
        u32 n;
        obj->flags &= ~1;
        s->comparisonIndicators[0].position.x = zero;
        s->comparisonIndicators[0].position.y = 8;
        n = s->rowIndex + 20;
        s->allocationDescription = n;
        SceneCatalog_SetDescription(s, (s8)s->allocationDescription);
        Hud_SetInteractionCallback(s, Presentation_UpdateAllocationSelection, 0);
        MenuDisplay_SetPaletteOverrides(s, 1, 0, 0, zero);
    }
    {
        u16 *reg = &gDispCntShadow;
        s32 mask = ~0x200;
        mask &= *reg;
        *reg = mask;
    }
}

void SceneCatalog_SelectDescription(s32 selection)
{
    if (selection < 0) {
        MessageWindow_Clear();
    } else {
        MessageWindow_SetDisplayMode(1);
        MessageWindow_ShowMessage(selection);
    }
}

s32 SceneCatalog_GetMappedState(void)
{
    s32 state = (s16)gGameState->unk12;
    if (state == 1)
        return 2;
    if (state == 2)
        return 1;
    return 0;
}

s32 SceneCatalog_MapSelectionMask(s32 selection)
{
    if (selection == 0)
        return 4;
    if (selection == 1)
        return 2;
    return 1;
}

void SceneCatalog_SaveOptions(struct SceneHudState *s)
{
    struct Player *player;
    s32 mapped;
    gGameState->values08[4] = s->savedSettingA;
    mapped = SceneCatalog_MapSelectionMask(s->savedSettingB);
    {
        u16 *option = &gGameState->unk12;
        *option = mapped;
    }
    {
        s16 *option = &gGameState->unk14;
        *option = s->savedSettingC;
    }
    GameState_BackupRegionRange(&gGameState->values08[4], 2);
    GameState_BackupRegionRange(&gGameState->unk12, 2);
    GameState_BackupRegionRange(&gGameState->unk14, 2);
    player = s->actionOwner;
    if (player->scriptStatus[1] == 0)
        player->catalogSavedOption = gGameState->values08[4];
}

void SelectionPanel_CaptureSettings(struct SceneHudState *p)
{
    p->settingsRow = 0;
    p->savedSettingA = gGameState->values08[4];
    p->savedSettingB = SceneCatalog_GetMappedState();
    p->savedSettingC = gGameState->unk14;
}

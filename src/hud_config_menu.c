#include "hud_config_menu.h"

void TextWindow_SetBounds(u32, u32, u32, u32, u32);
void TextWindow_StartPlayback(s32);
void PauseMenu_HidePromptIcons(void *);
void Hud_SetInteractionCallback(void *, void (*)(struct SceneHudState *), int);
void PauseMenu_SetPairedIcons(void *, u32, s32);
void MenuDisplay_SetPaletteOverrides(void *, s32, s32, s32, s32);
void PauseMenu_CloseTextWindows(void *);
void MessageWindow_Clear(void);
void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_RequestHide(void);
void TextWindow_Clear(u32);
void MessageWindow_SetBounds(u32, u32, u32, u32);
void Hud_SetupBackgroundLayer(void *, u32, u32, u32, u32);
void PauseMenu_HideAllSprites(void *);
void PauseMenu_LoadSpriteKeyframe(void *, u32, u32);
void MessageWindow_SetMessageTable(const void *);
void MessageWindow_SetPalette(u32);
void PauseMenu_SetPromptIconPositions(void *, u32, s32);
void PauseMenu_SetConfirmationIndicator(void *, s32);
int PauseMenu_UpdateBinaryChoice(void *);
void Power_EnterSleep(void);

static inline void SelectionPanel_HideWidget(struct SceneInputWidget *widget)
{
    widget->flags |= 1;
}

static inline void SelectionPanel_ShowWidget(struct SceneInputWidget *widget)
{
    widget->flags &= ~1;
}

void SelectionPanel_UpdateCursor(struct SceneHudState *menu, s32 visible)
{
    if (visible == 0) {
        SelectionPanel_HideWidget(&menu->categoryWidgets[5]);
        SelectionPanel_HideWidget(&menu->settingsArrows[0]);
        SelectionPanel_HideWidget(&menu->settingsArrows[1]);
    } else {
        u8 *row;
        menu->categoryWidgets[5].flags &= ~1;
        menu->categoryWidgets[5].position.x = 0;
        row = &menu->settingsRow;
        menu->categoryWidgets[5].position.y = *row * 16 + 0x18;
        SelectionPanel_ShowWidget(&menu->settingsArrows[0]);
        SelectionPanel_ShowWidget(&menu->settingsArrows[1]);
        menu->settingsArrows[0].position.x = 0x80;
        menu->settingsArrows[1].position.x = 0xd8;
        {
            s32 y = *row * 16 + 0x18;
            menu->settingsArrows[0].position.y = y;
            menu->settingsArrows[1].position.y = y;
        }
    }
    Anim_LoadKeyframe(&menu->settingsValues[0], menu->widgetResource, menu->savedSettings[0] + 0x4d,
                      1);
    Anim_LoadKeyframe(&menu->settingsValues[1], menu->widgetResource, menu->savedSettings[1] + 0x4d,
                      1);
    {
        s8 first = 0x4e;
        Anim_LoadKeyframe(&menu->settingsValues[2], menu->widgetResource,
                          (u16)(first - menu->savedSettings[2]), 1);
    }
}

void Hud_SetConfigFirstValueBounds(struct SceneHudState *menu)
{
    TextWindow_SetBounds(menu->settingsWindows[0][1], 0x11, 3, 0xf, 2);
}

void Hud_SetConfigSecondValueBounds(struct SceneHudState *menu)
{
    TextWindow_SetBounds(menu->settingsWindows[1][1], 0x11, 5, 0xA, 1);
}

void Hud_SetConfigThirdValueBounds(struct SceneHudState *menu)
{
    TextWindow_SetBounds(menu->settingsWindows[2][1], 0x11, 7, 0xA, 1);
}

void MenuSelection_RefreshOptionText(struct SceneHudState *menu)
{
    u8 *row = &menu->settingsRow;
    if (*row == 0) {
        s32 *id = &menu->settingsWindows[0][1];
        TextWindow_SetScript(*id, menu->optionUpperEScript);
        TextWindow_Refresh(*id, menu->savedSettings[0] + 28);
        Hud_SetConfigFirstValueBounds(menu);
        TextWindow_StartPlayback(*id);
    }
    if (*row == 1) {
        s32 *id = &menu->settingsWindows[1][1];
        TextWindow_SetScript(*id, menu->optionUpperEScript);
        TextWindow_Refresh(*id, menu->savedSettings[1] + 31);
        Hud_SetConfigSecondValueBounds(menu);
        TextWindow_StartPlayback(*id);
    }
    if (*row == 2) {
        s32 *id = &menu->settingsWindows[2][1];
        TextWindow_SetScript(*id, menu->optionUpperEScript);
        TextWindow_Refresh(*id, 35 - menu->savedSettings[2]);
        Hud_SetConfigThirdValueBounds(menu);
        TextWindow_StartPlayback(*id);
    }
}

void MenuSelection_RefreshAllOptionText(struct SceneHudState *menu)
{
    {
        s32 *id = &menu->settingsWindows[0][1];
        TextWindow_SetScript(*id, menu->optionUpperEScript);
        TextWindow_Refresh(*id, menu->savedSettings[0] + 28);
        Hud_SetConfigFirstValueBounds(menu);
        TextWindow_StartPlayback(*id);
    }
    {
        s32 *id = &menu->settingsWindows[1][1];
        TextWindow_SetScript(*id, menu->optionUpperEScript);
        TextWindow_Refresh(*id, menu->savedSettings[1] + 31);
        Hud_SetConfigSecondValueBounds(menu);
        TextWindow_StartPlayback(*id);
    }
    {
        s32 *id = &menu->settingsWindows[2][1];
        TextWindow_SetScript(*id, menu->optionUpperEScript);
        TextWindow_Refresh(*id, 35 - menu->savedSettings[2]);
        Hud_SetConfigThirdValueBounds(menu);
        TextWindow_StartPlayback(*id);
    }
}

s32 MenuSelection_ProcessOptionInput(struct SceneHudState *menu)
{
    u32 keys = gPadState[1];
    u16 up;
    u32 m;
    u8 *row;
    if (keys & 0x10) {
        row = &menu->settingsRow;
        if (*row > 1)
            goto toggle;
        {
            u8 *option = &menu->savedSettings[0];
            option += *row;
            if (*option > 1)
                *option = 0;
            else
                *option += 1;
            goto changed;
        }
    }
    if (!(keys & 0x20))
        goto vertical;
    row = &menu->settingsRow;
    if (*row <= 1) {
        {
            u8 *option = &menu->savedSettings[0];
            option += *row;
            if (*option == 0)
                *option = 2;
            else
                *option -= 1;
            goto changed;
        }
    }
toggle: {
    u8 *option = &menu->savedSettings[0];
    option += *row;
    *option = 1 - *option;
}
changed:
    Sound_PlaySong(0x19A);
    MenuSelection_RefreshOptionText(menu);
    return 1;
vertical:
    m = 0x40;
    up = keys & m;
    if (up) {
        row = &menu->settingsRow;
        if (*row == 0)
            *row = 2;
        else
            *row -= 1;
    } else {
        m = 0x80;
        m &= keys;
        if (m) {
            row = &menu->settingsRow;
            if (*row == 2)
                *row = up;
            else
                *row += 1;
        } else
            goto none;
    }
    Sound_PlaySong(0xDC);
    return 1;
none:
    return 0;
}

void SelectionPanel_ConfirmSettings(struct SceneHudState *menu)
{
    if (gPadState[1] & 1) {
        Sound_PlaySong(0xDD);
        SelectionPanel_UpdateCursor(menu, 1);
        SceneCatalog_SelectDescription(6);
        PauseMenu_HidePromptIcons(menu);
        Hud_SetInteractionCallback(menu, PauseMenu_UpdateConfigChoice, 1);
    }
}

void PauseMenu_UpdateConfigChoice(struct SceneHudState *menu)
{
    if (gPadState[1] & 1) {
        Sound_PlaySong(0xDD);
        SceneCatalog_SaveOptions(menu);
        SelectionPanel_UpdateCursor(menu, 0);
        SceneCatalog_SelectDescription(7);
        Hud_SetInteractionCallback(menu, SelectionPanel_WaitConfirmDelay, 1);
    } else if (gPadState[1] & 2) {
        Sound_PlaySong(0xDE);
        SelectionPanel_CaptureSettings(menu);
        SelectionPanel_UpdateCursor(menu, 0);
        SceneCatalog_SelectDescription(5);
        PauseMenu_SetPairedIcons(menu, 1, -1);
        MenuSelection_RefreshAllOptionText(menu);
        Hud_SetInteractionCallback(menu, SelectionPanel_ConfirmSettings, 0);
    } else {
        if (MenuSelection_ProcessOptionInput(menu))
            SelectionPanel_UpdateCursor(menu, 1);
        Anim_Step(&menu->categoryWidgets[5], menu->widgetResource);
    }
}

void SelectionPanel_WaitConfirmDelay(struct SceneHudState *menu)
{
    if (++menu->actionTransferDelay > 59) {
        SceneCatalog_SelectDescription(5);
        PauseMenu_SetPairedIcons(menu, 1, -1);
        Hud_SetInteractionCallback(menu, SelectionPanel_ConfirmSettings, 0);
    }
}

void PauseMenu_InitConfigChoice(struct SceneHudState *menu)
{
    SelectionPanel_CaptureSettings(menu);
    menu->settingsValues[0].flags |= 1;
    menu->settingsValues[1].flags |= 1;
    menu->settingsValues[2].flags |= 1;
    Anim_LoadKeyframe(&menu->categoryWidgets[5], menu->widgetResource, 0x47, 0);
    menu->categoryWidgets[5].position.x = 0;
    menu->categoryWidgets[5].position.y = 0x18;
    menu->categoryWidgets[5].position.z = 0;
    MenuDisplay_SetPaletteOverrides(menu, 1, 0, 0, 0);
    SelectionPanel_UpdateCursor(menu, 0);
}

void Hud_InitConfigMenu(struct SceneHudState *menu)
{
    u32 two;
    u32 *firstLabel;
    u32 *secondLabel;
    u32 *thirdLabel;
    u32 first;
    PauseMenu_CloseTextWindows(menu);
    MessageWindow_Clear();
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_RequestHide();
    TextWindow_Clear(menu->primaryTextWindow);
    TextWindow_Clear(menu->timedMessageWindow);
    MessageWindow_SetBounds(1, 0x10, 0x1D, 2);
    Hud_SetupBackgroundLayer(menu, menu->panelResource, 0, 10, menu->panelConfig);
    PauseMenu_HideAllSprites(menu);
    PauseMenu_LoadSpriteKeyframe(menu, 0, 0x58);
    PauseMenu_InitConfigChoice(menu);
    firstLabel = (u32 *)&menu->settingsWindows[0][0];
    TextWindow_SetScript(*firstLabel, menu->optionUpperEScript);
    TextWindow_Refresh(*firstLabel, 0x19);
    first = *firstLabel;
    two = 2;
    TextWindow_SetBounds(first, 2, 3, 10, two);
    TextWindow_StartPlayback(*firstLabel);
    secondLabel = (u32 *)&menu->settingsWindows[1][0];
    TextWindow_SetScript(*secondLabel, menu->optionUpperEScript);
    TextWindow_Refresh(*secondLabel, 0x1A);
    TextWindow_SetBounds(*secondLabel, 2, 5, 0x15, two);
    TextWindow_StartPlayback(*secondLabel);
    thirdLabel = (u32 *)&menu->settingsWindows[2][0];
    TextWindow_SetScript(*thirdLabel, menu->optionUpperEScript);
    TextWindow_Refresh(*thirdLabel, 0x1B);
    TextWindow_SetBounds(*thirdLabel, 2, 7, 11, two);
    TextWindow_StartPlayback(*thirdLabel);
    MenuSelection_RefreshAllOptionText(menu);
    MessageWindow_SetMessageTable(menu->optionUpperEScript);
    MessageWindow_SetPalette(0xEFDA);
    SceneCatalog_SelectDescription(5);
    PauseMenu_SetPromptIconPositions(menu, 0, -12);
    PauseMenu_SetPairedIcons(menu, 1, -1);
    Hud_SetInteractionCallback(menu, SelectionPanel_ConfirmSettings, 0);
}

void PauseMenu_BeginSleepPrompt(struct SceneHudState *menu)
{
    s32 *window;
    if (gPadState[1] & 1) {
        Sound_PlaySong(0xDD);
        PauseMenu_SetConfirmationIndicator(menu, 0);
        SceneCatalog_SelectDescription(0x12);
        PauseMenu_HidePromptIcons(menu);
        Hud_SetInteractionCallback(menu, SelectionPanel_ProcessChoice, 1);
        MenuDisplay_SetPaletteOverrides(menu, 0, 0, 1, 0);
        window = &menu->timedMessageWindow;
        TextWindow_SetBounds(*window, 4, 6, 0x16, 6);
        TextWindow_SetScript(*window, menu->optionUpperEScript);
        TextWindow_Refresh(*window, 0x10);
        TextWindow_StartPlayback(*window);
    }
}

void SelectionPanel_ProcessChoice(struct SceneHudState *menu)
{
    int choice = PauseMenu_UpdateBinaryChoice(menu);
    if (choice == 0) {
        Sound_PlaySong(0xDE);
        SceneCatalog_SelectDescription(0x11);
        TextWindow_Clear(menu->timedMessageWindow);
        PauseMenu_SetPairedIcons(menu, 1, -1);
        Hud_SetInteractionCallback(menu, PauseMenu_BeginSleepPrompt, 0);
    } else if (choice == 1) {
        Sound_PlaySong(0xDD);
        SceneCatalog_SelectDescription(0x13);
        Hud_SetInteractionCallback(menu, SelectionPanel_WaitThenSleep, 1);
    }
}

void SelectionPanel_WaitThenSleep(struct SceneHudState *menu)
{
    gSceneDialogActive = 1;
    if (++menu->actionTransferDelay > 119) {
        SceneCatalog_SelectDescription(0x11);
        TextWindow_Clear(menu->timedMessageWindow);
        PauseMenu_SetPairedIcons(menu, 1, -1);
        Hud_SetInteractionCallback(menu, PauseMenu_BeginSleepPrompt, 0);
        menu->pageInputCooldown = 30;
        Power_EnterSleep();
    }
}

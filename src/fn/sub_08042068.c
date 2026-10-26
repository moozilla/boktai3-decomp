#include "modifier_list.h"
#include "ram.h"
void ModifierMenu_ValidateSavedSelection(struct ModifierMenu *);
void ModifierMenu_LoadSavedSelection(struct ModifierMenu *);
void ModifierMenu_LoadTextSources(struct ModifierMenu *);
void ModifierMenu_InitializePreview(struct ModifierMenu *);
void ScreenTransition_SendCommand(u32, u32, u32, u32, u32, u32, u32);
void ModifierMenu_LoadBackground(struct ModifierMenu *, s32);
u32 ModifierMenu_StartBackgroundScript(struct ModifierMenu *);
void ModifierMenu_InitializeGraphics(struct ModifierMenu *);
void ModifierMenu_InitHeaderBackdrop(struct ModifierMenu *);
void ModifierMenu_InitSelectionSnapshot(void *, void *);
s32 ModifierMenu_CreateTextWindows(struct ModifierMenu *);
void ScrollingDialogue_RequestShow(void);
void ModifierMenu_InitSavedCursors(struct ModifierMenu *);
s32 ModifierMenu_Initialize(struct ModifierMenu *menu)
{
    u32 mask;
    u32 flags;
    menu->unknown116 = 0;
    ModifierMenu_ValidateSavedSelection(menu);
    ModifierMenu_LoadSavedSelection(menu);
    mask = 0x800;
    flags = (gEngineFlags | gScenePauseFlags) & mask;
    /* The masked value is zero or 0x800: negation extracts its nonzero flag. */
    menu->specialHeader = Script_GetKeywordValue('E', (0 - flags) >> 31);
    menu->script = (void *)Script_GetKeywordValue('e', 0);
    if (Script_GetKeywordValue('f', 1)) {
        menu->inputDelay = 32;
        ScreenTransition_SendCommand(2, 5, 4, 4, 4, 0xFFFF, 0);
    } else {
        menu->inputDelay = 0;
    }
    ModifierMenu_LoadTextSources(menu);
    ModifierMenu_LoadBackground(menu, 0);
    ModifierMenu_StartBackgroundScript(menu);
    ModifierMenu_InitializeGraphics(menu);
    ModifierMenu_InitHeaderBackdrop(menu);
    ModifierMenu_InitSelectionSnapshot((u8 *)menu + 0x3C, menu->tilemap);
    if (ModifierMenu_CreateTextWindows(menu) < 0)
        return -1;
    ScrollingDialogue_RequestShow();
    menu->dirtyFlags = -1;
    ModifierMenu_InitializePreview(menu);
    ModifierMenu_InitSavedCursors(menu);
    return 0;
}

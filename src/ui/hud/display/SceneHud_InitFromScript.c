#include "global.h"
#include "engine/ram.h"
#include "engine/engine_globals.h"
#include "player/player.h"
#include "ui/pause/scene_hud_display.h"
extern u8 gUnk_08E60F0C[];
void Inventory_InitEquipmentEntryFromDescriptor(u8 *, struct SceneValueEquipment *);
void SceneHud_ReadScriptOptions(struct SceneHudState *);
void SceneHud_ReadPageVisibility(struct SceneHudState *);
void Hud_ReadScriptQOption(struct SceneHudState *);
void Hud_LoadLayoutResources(struct SceneHudState *);
void SelectionPanel_RefreshAttachedPanel(struct SceneHudState *);
void sub_08176F70(struct SceneHudState *);
u32 TextWindow_Create(u32, u32, u32, u32);
void sub_08167FC0(struct SceneHudState *);
void PauseMenu_RefreshStatusPanel(struct SceneHudState *);
void Hud_RefreshActivePanel(struct SceneHudState *);
void Hud_OnUnpausedRefresh(void);
void FieldHud_RefreshModeEight(void);
void Hud_UpdateEquipmentPrompt(void);
void Hud_SetUpdateCallback(struct SceneHudState *, void (*)(void));
void InventoryPanel_InitCallbackGroups(struct SceneHudState *);
void SceneHud_RebuildTertiaryOptions(struct SceneHudState *);
struct DarkLoanScreen *DarkLoan_CreateHiddenDisplay(u32, u32, u32, const void *);

// confidence=low: initializes the scene HUD and its script-configured panels.
u32 SceneHud_InitFromScript(struct SceneHudState *hud)
{
    struct Player *player;
    hud->initializationFlag = 0;
    Bg_SetPriority(0, 0);
    hud->initializationState = 0;
    hud->mode = gSceneHudMode;
    player = gPlayers[0];
    hud->actionOwner = player;
    Inventory_InitEquipmentEntryFromDescriptor(gUnk_08E60F0C, &hud->alternateEquipment);
    if (Script_SeekToKeyword('N'))
        hud->controlMode = Script_GetValue();
    else
        hud->controlMode = 0;
    if (hud->controlMode != 8)
        SceneHud_ReadScriptOptions(hud);
    hud->playerValueEnabled = 1;
    SceneHud_ReadPageVisibility(hud);
    Hud_ReadScriptQOption(hud);
    Hud_LoadLayoutResources(hud);
    SelectionPanel_RefreshAttachedPanel(hud);
    sub_08176F70(hud);
    if (Script_SeekToKeyword('d'))
        hud->accountCompletionScript = Script_GetValue();
    else
        hud->accountCompletionScript = 0;
    if (Script_SeekToKeyword('V'))
        hud->loanOptionV = Script_GetValue();
    else
        hud->loanOptionV = 0;
    if (Script_SeekToKeyword('O'))
        hud->selectedQuantity = Script_GetValue();
    else
        hud->selectedQuantity = 0;
    if (Script_SeekToKeyword('R'))
        hud->pageExitScript = Script_GetValue();
    else
        hud->pageExitScript = 0;
    if (Script_SeekToKeyword('D'))
        hud->effectScript = Script_GetValue();
    else
        hud->effectScript = 0;
    if (!(gEngineFlags & 0x1000)) {
        hud->primaryTextWindow = TextWindow_Create(6, 4, 10, 2);
        hud->timedMessageWindow = TextWindow_Create(4, 6, 22, 6);
        hud->settingsWindows[0][0] = TextWindow_Create(2, 3, 10, 2);
        hud->settingsWindows[1][0] = TextWindow_Create(2, 5, 21, 2);
        hud->settingsWindows[2][0] = TextWindow_Create(2, 7, 11, 2);
        hud->settingsWindows[0][1] = TextWindow_Create(17, 3, 15, 2);
        hud->settingsWindows[1][1] = TextWindow_Create(13, 9, 10, 2);
        hud->settingsWindows[2][1] = TextWindow_Create(13, 12, 12, 2);
    }
    sub_08167FC0(hud);
    if (gEngineFlags & 0x1000) {
        PauseMenu_RefreshStatusPanel(hud);
        Hud_SetUpdateCallback(hud, Hud_OnUnpausedRefresh);
    } else if (hud->controlMode == 8) {
        Hud_SetUpdateCallback(hud, FieldHud_RefreshModeEight);
    } else {
        Hud_RefreshActivePanel(hud);
        Hud_SetUpdateCallback(hud, Hud_UpdateEquipmentPrompt);
    }
    InventoryPanel_InitCallbackGroups(hud);
    hud->actionCueSuppressed = 2;
    hud->previousCueGroup = 255;
    hud->previousCueSelection = -1;
    hud->previousPage = -1;
    hud->hideSpecialMarkers = 0;
    gFieldHud = (u8 *)hud;
    SceneHud_RebuildTertiaryOptions(hud);
    if ((gGameState->flags868 & 0x580) || (gEngineFlags & 0x3800))
        hud->loanDisplay = 0;
    else
        hud->loanDisplay =
            DarkLoan_CreateHiddenDisplay(hud->selectedQuantity, hud->accountCompletionScript,
                                         hud->loanOptionV, hud->optionLScript);
    return 0;
}

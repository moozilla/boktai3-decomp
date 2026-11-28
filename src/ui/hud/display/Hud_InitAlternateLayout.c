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

u32 Hud_InitAlternateLayout(struct SceneHudState *hud)
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
        hud->controlMode = 8;
    SceneHud_ReadPageVisibility(hud);
    Hud_ReadScriptQOption(hud);
    Hud_LoadLayoutResources(hud);
    SelectionPanel_RefreshAttachedPanel(hud);
    sub_08176F70(hud);
    hud->accountCompletionScript = 0;
    hud->loanOptionV = 0;
    hud->pageExitScript = 0;
    hud->effectScript = 0;
    hud->primaryTextWindow = TextWindow_Create(6, 4, 10, 2);
    hud->timedMessageWindow = TextWindow_Create(4, 6, 22, 6);
    hud->settingsWindows[0][0] = TextWindow_Create(2, 3, 10, 2);
    hud->settingsWindows[1][0] = TextWindow_Create(2, 5, 21, 2);
    hud->settingsWindows[2][0] = TextWindow_Create(2, 7, 11, 2);
    hud->settingsWindows[0][1] = TextWindow_Create(17, 3, 15, 2);
    hud->settingsWindows[1][1] = TextWindow_Create(13, 9, 10, 2);
    hud->settingsWindows[2][1] = TextWindow_Create(13, 12, 12, 2);
    sub_08167FC0(hud);
    Hud_SetUpdateCallback(hud, FieldHud_RefreshModeEight);
    InventoryPanel_InitCallbackGroups(hud);
    Hud_SetUpdateCallback(hud, FieldHud_RefreshModeEight);
    hud->actionCueSuppressed = 2;
    hud->previousCueGroup = 255;
    hud->previousCueSelection = -1;
    hud->previousPage = -1;
    gFieldHud = (u8 *)hud;
    return 0;
}

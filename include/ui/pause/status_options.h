#ifndef GUARD_SCENE_CATALOG_H
#define GUARD_SCENE_CATALOG_H
#include "ui/pause/scene_hud_display.h"
#include "player/player.h"

void StatusOptions_SetDescription(struct SceneHudState *s, s32 selection);
void InventoryGrid_UpdateModifierPreview(struct SceneHudState *p);
void Presentation_UpdateAllocationSelection(struct SceneHudState *p);
void MenuSelection_ProcessConfirmation(struct SceneHudState *s);
s32 StatusOptions_GetLevelScaledValue(struct SceneHudState *s);
void MenuSelection_DrawNumericSummary(struct SceneHudState *s);
void StatusOptions_SetSelection(struct SceneHudState *s, u32 selection);
void StatusOptions_ResetTextWindow(struct SceneHudState *s);
void Hud_DrawStatusDayNightIndicators(struct SceneHudState *s);
void Hud_InitStatusMenu(struct SceneHudState *s);
void StatusOptions_SelectDescription(s32 selection);
s32 StatusOptions_GetMappedState(void);
s32 StatusOptions_MapSelectionMask(s32 selection);
void StatusOptions_SaveOptions(struct SceneHudState *s);
void SelectionPanel_CaptureSettings(struct SceneHudState *p);
#endif

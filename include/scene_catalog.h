#ifndef GUARD_SCENE_CATALOG_H
#define GUARD_SCENE_CATALOG_H
#include "scene_hud_display.h"
#include "player.h"

void SceneCatalog_SetDescription(struct SceneHudState *s, s32 selection);
void InventoryGrid_UpdateModifierPreview(struct SceneHudState *p);
void Presentation_UpdateAllocationSelection(struct SceneHudState *p);
void MenuSelection_ProcessConfirmation(struct SceneHudState *s);
s32 SceneCatalog_GetLevelScaledValue(struct SceneHudState *s);
void MenuSelection_DrawNumericSummary(struct SceneHudState *s);
void SceneCatalog_SetSelection(struct SceneHudState *s, u32 selection);
void SceneCatalog_ResetTextWindow(struct SceneHudState *s);
void Hud_DrawStatusDayNightIndicators(struct SceneHudState *s);
void Hud_InitStatusMenu(struct SceneHudState *s);
void SceneCatalog_SelectDescription(s32 selection);
s32 SceneCatalog_GetMappedState(void);
s32 SceneCatalog_MapSelectionMask(s32 selection);
void SceneCatalog_SaveOptions(struct SceneHudState *s);
void SelectionPanel_CaptureSettings(struct SceneHudState *p);
#endif

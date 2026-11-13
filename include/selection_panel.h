#ifndef GUARD_SELECTION_PANEL_H
#define GUARD_SELECTION_PANEL_H
#include "scene_catalog.h"
#include "runtime.h"

void SelectionPanel_UpdateCursor(struct SceneHudState *p, s32 visible);
void Hud_SetConfigFirstValueBounds(struct SceneHudState *p);
void Hud_SetConfigSecondValueBounds(struct SceneHudState *p);
void Hud_SetConfigThirdValueBounds(struct SceneHudState *p);
void MenuSelection_RefreshOptionText(struct SceneHudState *s);
void MenuSelection_RefreshAllOptionText(struct SceneHudState *s);
s32 MenuSelection_ProcessOptionInput(struct SceneHudState *s);
void SelectionPanel_ConfirmSettings(struct SceneHudState *p);
void PauseMenu_UpdateConfigChoice(struct SceneHudState *s);
void SelectionPanel_WaitConfirmDelay(struct SceneHudState *p);
void PauseMenu_InitConfigChoice(struct SceneHudState *s);
void Hud_InitConfigMenu(struct SceneHudState *s);
void PauseMenu_BeginSleepPrompt(struct SceneHudState *s);
void SelectionPanel_ProcessChoice(struct SceneHudState *p);
void SelectionPanel_WaitThenSleep(struct SceneHudState *p);
#endif

#ifndef GUARD_SCENE_DIALOG_H
#define GUARD_SCENE_DIALOG_H
#include "selection_panel.h"

void PauseMenu_InitSleepTab(struct SceneHudState *s);
void SelectionPanel_ConfirmTransition(struct SceneHudState *p);
void SelectionPanel_ProcessFlaggedChoice(struct SceneHudState *p);
void SelectionPanel_ProcessSecondChoice(struct SceneHudState *p);
void SceneDialog_BeginTimedMessage(struct SceneHudState *p);
void SceneDialog_SelectResultPhase(struct SceneHudState *p);
void SceneDialog_FinishSuccessfulResult(struct SceneHudState *p);
void SceneDialog_FinishFailedResult(struct SceneHudState *p);
void PauseMenu_EnterPrompt(struct SceneHudState *s);
void SceneDialog_ConfigureVariantSlot(struct SceneHudState *p);
void PauseMenu_SaveDisplayState(struct SceneHudState *s);
void PauseMenu_RestoreDisplayState(struct SceneHudState *s);
s32 SceneDialog_FindNextEnabledEntry(struct SceneHudState *p, s32 current);
s32 SceneDialog_FindPreviousEnabledEntry(struct SceneHudState *p, s32 current);
void SceneDialog_UpdateEntryFade(struct SceneHudState *p);
void Hud_UpdateBlendRamp(struct SceneHudState *p);
void SceneHud_UpdatePageInput(struct SceneHudState *p);
#endif

#ifndef GUARD_MENU_PROMPT_H
#define GUARD_MENU_PROMPT_H

#include "menus/scene_hud/scene_hud_display.h"

void MenuPrompt_BeginPulseDisplay(u8 *hud);
void MenuPrompt_RampDisplay(struct SceneHudState *hud);
void MenuPrompt_UpdateChoice(struct SceneHudState *hud);
void MenuPrompt_FadeChoice(struct SceneHudState *hud);
void ScenePanel_UpdateDetail(struct SceneHudState *hud);
void MenuPrompt_FinishDelayedChoice(struct SceneHudState *hud);
void MenuPrompt_UpdatePageExit(u8 *hud);
void MenuPrompt_WaitForChildPhase(struct SceneHudState *hud);
void Hud_UpdateFirstPostScriptWait(u8 *hud);
void MenuPrompt_PulseDisplay(struct SceneHudState *hud);
s32 Hud_GetScaledRetryCount(void);
void MenuPrompt_ResumeChild(struct SceneHudState *hud);
void MenuPrompt_UpdateSelectionSprites(struct SceneHudState *hud);
void Hud_InitRetryPanel(u8 *hud);
void ScenePanel_BeginAccountMenu(struct SceneHudState *hud);

#endif

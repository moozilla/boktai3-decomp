#include "global.h"
void Hud_SetupBackgroundLayer(u8 *, u32, s32, s32, u32);
void ScenePanel_BeginSelection(u8 *);
void Hud_SetUpdateCallback(u8 *, void (*)(u8 *));
void MenuPrompt_RampDisplay(u8 *);

void sub_0816A578(u8 *hud)
{
    Hud_SetupBackgroundLayer(hud, *(u32 *)(hud + 0x24), 0, 3, *(u32 *)(hud + 0x34));
    ScenePanel_BeginSelection(hud);
    Hud_SetUpdateCallback(hud, MenuPrompt_RampDisplay);
}

#include "global.h"
#include "scene_hud_display.h"
int SceneInput_NavigateGridKeys(void *, int);
void SceneHud_GetActionIconPosition(void *, int);
void PauseInventory_RefreshConsumableDescription(void *);
void MenuDisplay_UpdateActionPrompt(void *, int);
void SceneHud_SetPairedIcons(void *, int, int);
void PauseInventory_ReturnToConsumableActions(void *);
void Hud_SaveGridSelection(void *);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void SceneHud_UpdateActionSwapSelection(void);
void Sound_PlaySong(u32);
void PauseInventory_UpdateConsumableOptionSelection(struct SceneHudState *p)
{
    u8 *input = p->actionInput;
    u32 keys;
    u32 one;
    if (SceneInput_NavigateGridKeys(input, 0xF0)) {
        SceneHud_GetActionIconPosition(&p->categoryWidgets[5].position, p->actionCursor);
        PauseInventory_RefreshConsumableDescription(p);
        MenuDisplay_UpdateActionPrompt(p, 3);
    }
    SceneHud_SetPairedIcons(p, 1, 4);
    keys = gPadState[1];
    if (keys & 2) {
        Sound_PlaySong(0xDE);
        PauseInventory_ReturnToConsumableActions(p);
    } else if ((one = 1, one &= keys)) {
        Sound_PlaySong(0xDD);
        Hud_SaveGridSelection(input);
        p->categoryWidgets[6].flags &= ~1;
        SceneHud_GetActionIconPosition(&p->categoryWidgets[6].position, p->actionSelection);
        Hud_SetInteractionCallback(p, SceneHud_UpdateActionSwapSelection, 1);
    }
}

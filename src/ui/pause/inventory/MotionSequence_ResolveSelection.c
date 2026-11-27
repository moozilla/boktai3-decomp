#include "ui/pause/scene_hud_display.h"
#define MotionSequence_SetCallback Hud_SetInteractionCallback
#include "global.h"
/* Tentative semantic names; field offsets are established by static accesses. */
int PauseMenu_UpdateBinaryChoice(void *);
void Sound_PlaySong(u32);
void PauseInventory_BeginConsumableDiscardSelection(void *, int);
void PauseMenu_AddSpriteFlags(void *, int, int);
s32 MessageWindow_Clear(void);
void SceneHud_PrepareActionTransferToSlot(void *, int, u32, int);
void GameState_ClearConsumableSlot(u32);
void PauseInventory_RefreshConsumableIcons(void *);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void PauseInventory_UpdateConsumableDiscardAnimation(void);
void MotionSequence_ResolveSelection(struct SceneHudState *p)
{
    int result = PauseMenu_UpdateBinaryChoice(p);
    if (result == 0) {
        Sound_PlaySong(0xDE);
        PauseInventory_BeginConsumableDiscardSelection(p, 0);
    } else if (result == 1) {
        PauseMenu_AddSpriteFlags(p, 5, 1);
        MessageWindow_Clear();
        SceneHud_PrepareActionTransferToSlot(p, 0, p->actionSelection, 0x24);
        GameState_ClearConsumableSlot(p->actionSelection);
        PauseInventory_RefreshConsumableIcons(p);
        Hud_SetInteractionCallback(p, PauseInventory_UpdateConsumableDiscardAnimation, 1);
    }
}

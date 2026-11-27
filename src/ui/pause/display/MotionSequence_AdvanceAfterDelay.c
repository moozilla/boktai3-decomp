#include "ui/pause/scene_hud_display.h"
#define MotionSequence_UpdateInterpolation PauseMenu_UpdateSpriteMotion
#define MotionSequence_SetCallback         Hud_SetInteractionCallback
#include "global.h"
/* Tentative semantic names; field offsets are established by static accesses. */
u32 PauseMenu_UpdateSpriteMotion(struct SceneHudState *, u32);
void PauseMenu_ApplyItemSlot(struct SceneHudState *, u32);
void SceneHud_UpdateTimedMessage(void);
void Hud_SetInteractionCallback(struct SceneHudState *, void (*)(void), int);
void Sound_PlaySong(u32);
void MotionSequence_AdvanceAfterDelay(struct SceneHudState *p)
{
    if (PauseMenu_UpdateSpriteMotion(p, 0)) {
        if (++p->actionTransferDelay > 7) {
            p->motionWidgets[0].flags |= 1;
            PauseMenu_ApplyItemSlot(p, p->actionSelection);
            Hud_SetInteractionCallback(p, SceneHud_UpdateTimedMessage, 1);
            Sound_PlaySong(0x10D);
        }
    }
}

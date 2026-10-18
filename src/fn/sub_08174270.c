#include "global.h"
#include "scene_hud_display.h"
void PauseInventory_BeginItemDiscardSelection(void *, s32);
void SceneHud_TickEquipmentFeedback(struct SceneHudState *p)
{
    if (p->actionTransferDelay == 0)
        Sound_PlaySong(0x10e);
    if (++p->actionTransferDelay <= 11) {
        if ((p->actionTransferDelay >> 1) & 1)
            p->actionFeedbackY = 0x68;
        else
            p->actionFeedbackY = 0x68 - ((20 - p->actionTransferDelay) >> 3);
    } else if (p->actionTransferDelay <= 15) {
        p->actionFeedbackY = 0x68;
    } else
        PauseInventory_BeginItemDiscardSelection(p, 0);
}

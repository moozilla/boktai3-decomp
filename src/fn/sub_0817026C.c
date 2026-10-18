#define SceneHud_UpdateActionFeedback sub_0817026C

#include "global.h"
#include "scene_hud_display.h"
void sub_0816D98C(void *);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void PauseMenu_OnItemUseSelectionUpdate(void);
void sub_0816CDB8(void *, int, int);
void sub_0816D920(void *, int);
void sub_0816D868(void *);
void PauseMenu_ClearSpriteFlags(void *, s32, u32);
void sub_081663A0(void *, int, int, int, int);
void sub_0817026C(struct SceneHudState *p)
{
    if (p->actionTransferDelay == 0)
        Sound_PlaySong(0x10E);
    if (++p->actionTransferDelay < 12) {
        if ((p->actionTransferDelay >> 1) & 1)
            p->actionFeedbackY = 104;
        else
            p->actionFeedbackY = 104 - ((20 - p->actionTransferDelay) >> 3);
    } else if (p->actionTransferDelay < 16)
        p->actionFeedbackY = 104;
    else {
        sub_0816D98C(p);
        Hud_SetInteractionCallback(p, PauseMenu_OnItemUseSelectionUpdate, 1);
        sub_0816CDB8(p->actionInput, 1, 0);
        sub_0816D920((u8 *)p + 0x1620, p->actionCursor);
        sub_0816D868(p);
        PauseMenu_ClearSpriteFlags(p, 5, 1);
        sub_081663A0(p, 1, 0, 0, 0);
    }
}

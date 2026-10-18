#define SceneHud_ResolveActionConfirmation sub_0817014C

#include "global.h"
#include "scene_hud_display.h"
int sub_08165A30(void *);
void sub_0816D98C(void *);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void PauseMenu_OnItemUseSelectionUpdate(void);
void PauseMenu_OnItemSelectionTransitionUpdate(void);
void sub_0816CDB8(void *, int, int);
void sub_0816D920(void *, int);
void sub_0816D868(void *);
void PauseMenu_ClearSpriteFlags(void *, s32, u32);
void PauseMenu_AddSpriteFlags(void *, int, int);
void sub_081663A0(void *, int, int, int, int);
s32 MessageWindow_Clear(void);
void sub_0816DA58(void *, int, int, int);
void PauseMenu_ClearItemSlot(void *, u32);
void sub_0817014C(struct SceneHudState *p)
{
    int r = sub_08165A30(p);
    u8 *selection;
    if (r == 0) {
        Sound_PlaySong(222);
        sub_0816D98C(p);
        Hud_SetInteractionCallback(p, PauseMenu_OnItemUseSelectionUpdate, 1);
        sub_0816CDB8(p->actionInput, 1, 0);
        sub_0816D920((u8 *)p + 0x1620, p->actionCursor);
        sub_0816D868(p);
        PauseMenu_ClearSpriteFlags(p, 5, 1);
        sub_081663A0(p, 1, 0, 0, r);
    } else if (r == 1) {
        PauseMenu_AddSpriteFlags(p, 5, 1);
        MessageWindow_Clear();
        selection = &p->actionSelection;
        sub_0816DA58(p, 0, *selection, 36);
        PauseMenu_ClearItemSlot(p, *selection);
        sub_0816D98C(p);
        Hud_SetInteractionCallback(p, PauseMenu_OnItemSelectionTransitionUpdate, 1);
    }
}

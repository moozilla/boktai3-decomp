
#ifdef NONMATCHING
// fuzzy: 91.2%
// foundry draft (worker best 90.2%); what differs is mostly register allocation and instruction order
#define SceneHud_FinishActionSlide sub_0817061C
#define SceneHud_BlendPosition PauseMenu_SmoothPoint
#include "global.h"
#include "scene_hud_display.h"
void Sound_PlaySong(u32);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void PauseMenu_AddSpriteFlags(void *, int, int);
void SceneHud_BlendPosition(void *, void *, s32);
void MenuDisplay_SetPaletteOverrides(void *, int, int, int, int);
void MenuDisplay_UpdateActionPrompt(void *, int);
void sub_0816CDB8(void *, int, int);
void PauseMenu_SwapItemSlots(void *, u32, u32);
void SceneHud_ShowActionPreview(void *);
void SceneHud_GetActionIconPosition(void *, int);
void SceneHud_RefreshActionIcons(void *);
void PauseMenu_OnSwapSelectionUpdate(void);
void sub_0817061C(struct SceneHudState *s)
{
    u16 *timer = &s->actionTransferDelay;
    if (!*timer)
        Sound_PlaySong(0x38a);
    (*timer)++;
    if (*timer <= 11) {
        s32 n = 12 - *timer;
        SceneHud_BlendPosition(&s->motionWidgets[0].position, &s->actionTargets[0], n);
        SceneHud_BlendPosition(&s->motionWidgets[1].position, &s->actionTargets[1], n);
    } else {
        u32 *q = &s->motionWidgets[0].flags;
        u32 v = *q, m = 1;
        *q = v | m;
        s->motionWidgets[1].flags |= m;
        PauseMenu_SwapItemSlots(s, s->actionSelection, s->actionCursor);
        SceneHud_RefreshActionIcons(s);
        Hud_SetInteractionCallback(s, PauseMenu_OnSwapSelectionUpdate, 1);
        sub_0816CDB8(s->actionInput, 1, 0);
        SceneHud_GetActionIconPosition((u8 *)s + 0x1620, s->actionCursor);
        SceneHud_ShowActionPreview(s);
        MenuDisplay_UpdateActionPrompt(s, 4);
        PauseMenu_AddSpriteFlags(s, 6, 1);
        MenuDisplay_SetPaletteOverrides(s, 1, 0, 0, 0);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0817061C);
#endif

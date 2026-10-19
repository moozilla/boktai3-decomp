#include "global.h"
#include "scene_hud_display.h"
#ifdef NONMATCHING
// fuzzy: 98.3%
// foundry draft (worker best 98.3%); what differs is mostly register allocation and instruction order
#define SceneHud_FinishStatBRefill sub_0816F5B0
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void MenuDisplay_SetPaletteOverrides(void *, int, int, int, int);
void MenuDisplay_UpdateActionPrompt(void *, int);
void sub_0816701C(void *, int, int, int, int, int);
void sub_081672FC(void *);
void Hud_SaveGridSelection(void *);
void sub_0816CDB8(void *, int, int);
void PauseMenu_ClearItemSlot(void *, u32);
void SceneHud_ShowActionPreview(void *);
void SceneHud_GetActionIconPosition(void *, int);
void SceneHud_RefreshActionIcons(void *);
void SceneHud_UpdateActionSelection(void);
static inline s32 MaskFrame(s32 mask, u16 frame)
{
    return mask & frame;
}
void sub_0816F5B0(struct SceneHudState *s)
{
    u16 frame;
    u8 *input;
    Anim_Step(&s->motionWidgets[14], s->actionResultResources[s->actionTransferBonus]);
    frame = s->actionTransferDelay;
    if (MaskFrame(7, frame) < 7 - (frame >> 3))
        s->motionWidgets[14].flags &= ~1;
    else
        s->motionWidgets[14].flags |= 1;
    if (++s->actionTransferDelay > 55) {
        PauseMenu_ClearItemSlot(s, s->actionSelection);
        SceneHud_RefreshActionIcons(s);
        Hud_SetInteractionCallback(s, SceneHud_UpdateActionSelection, 1);
        input = s->actionInput;
        sub_0816CDB8(input, 1, 0);
        SceneHud_GetActionIconPosition(&s->categoryWidgets[5].position, s->actionCursor);
        SceneHud_ShowActionPreview(s);
        Hud_SaveGridSelection(input);
        sub_081672FC(s);
        MenuDisplay_UpdateActionPrompt(s, 4);
        sub_0816701C(s, 0, 0, 0, 1, 0);
        MenuDisplay_SetPaletteOverrides(s, 1, 0, 0, 0);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0816F5B0);
#endif

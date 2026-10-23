#include "global.h"
#include "scene_hud_display.h"

#define SceneHud_BlendPosition PauseMenu_SmoothPoint
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
void SceneHud_FinishActionSlide(struct SceneHudState *s)
{
    u16 *timer = &s->actionTransferDelay;
    if (!(*timer)) {
        Sound_PlaySong(0x38a);
    }
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
        {
            u32 sel = s->actionSelection;
            unsigned long long off = 0x4387;
            u8 *a;
            a = (u8 *)s + off;
            PauseMenu_SwapItemSlots(s, sel, *a);
        }
        SceneHud_RefreshActionIcons(s);
        Hud_SetInteractionCallback(s, PauseMenu_OnSwapSelectionUpdate, 1);
        {
            u32 o1 = 0x4384;
            u8 *b;
            u8 *in;
            u8 *pp;
            in = (u8 *)s + o1;
            sub_0816CDB8(in, 1, 0);
            {
                u32 o3 = 0x1620;
                pp = (u8 *)s + o3;
            }
            {
                u32 o2 = 0x4387;
                b = (u8 *)s + o2;
            }
            SceneHud_GetActionIconPosition(pp, *b);
        }
        SceneHud_ShowActionPreview(s);
        MenuDisplay_UpdateActionPrompt(s, 4);
        PauseMenu_AddSpriteFlags(s, 6, 1);
        MenuDisplay_SetPaletteOverrides(s, 1, 0, 0, 0);
    }
}

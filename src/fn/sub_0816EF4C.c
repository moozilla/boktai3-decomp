#include "global.h"

/* tentative: moves two meter values toward their goals each frame, then restores the action HUD */

void Sound_PlaySong(int);
void Sound_StopSong(int);
s32 Div(s32, s32);
void SceneHud_RefreshActionIcons(void *);
void SceneHud_UpdateActionSelection(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void sub_0816CDB8(void *, int, int);
void SceneHud_GetActionIconPosition(void *, int);
void SceneHud_ShowActionPreview(void *);
void Hud_SaveGridSelection(void *);
void InventoryPanel_ShowStatusIcons(void *);
void MenuDisplay_UpdateActionPrompt(void *, int);
void InventoryPanel_DrawStatComparison(void *, int, int, int, int, int);
void MenuDisplay_SetPaletteOverrides(void *, int, int, int, int);
void sub_0816D634(void *, u32);
void Hud_DrawDefaultHealthMeter(void *, u32, u32, u32);
void Hud_DrawDefaultEnergyMeter(void *, s32, s32, s32, s32, s32);
struct MenuFlickerTarget {
    u8 unknown0[0x424];
    u16 x, xMax, y, yMax;
};
struct MenuFlickerView {
    u8 unknown0[0xa40];
    struct MenuFlickerTarget *target;
    u8 unknowna44[10];
    u16 timer;
};
void MenuTransition_AnimateMetersToTarget(u8 *p)
{
    s32 done;
    s32 step;
    u8 *q;
    if (((struct MenuFlickerView *)p)->timer == 0) {
        Sound_PlaySong(0x259);
        ((struct MenuFlickerView *)p)->timer++;
    }
    done = 0;
    step = Div(((struct MenuFlickerView *)p)->target->xMax, 120);
    if (step == 0)
        step = 1;
    {
        u16 *v = &((struct MenuFlickerView *)p)->target->x;
        s32 cur = *v;
        s32 goal = *(u16 *)(p + 0x486a);
        if (cur > goal + step)
            *v = cur - step;
        else if (cur < goal - step)
            *v = cur + step;
        else {
            *v = goal;
            done = 1;
        }
    }
    step = Div(((struct MenuFlickerView *)p)->target->yMax, 120);
    if (step == 0)
        step = 1;
    {
        u16 *v = &((struct MenuFlickerView *)p)->target->y;
        s32 cur = *v;
        s32 goal = *(u16 *)(p + 0x486e);
        if (cur > goal + step)
            *v = cur - step;
        else if (cur < goal - step)
            *v = cur + step;
        else {
            *v = goal;
            done++;
        }
    }
    Hud_DrawDefaultHealthMeter(p, 1, 5, 3);
    Hud_DrawDefaultEnergyMeter(p, 1, 5, 4, ((struct MenuFlickerView *)p)->target->y,
                               ((struct MenuFlickerView *)p)->target->yMax);
    if (done > 1) {
        Sound_StopSong(0x259);
        sub_0816D634(p, p[0x438a]);
        SceneHud_RefreshActionIcons(p);
        Hud_SetInteractionCallback(p, SceneHud_UpdateActionSelection, 1);
        q = p + 0x4384;
        sub_0816CDB8(q, 1, 0);
        SceneHud_GetActionIconPosition(p + 0x1620, p[0x4387]);
        SceneHud_ShowActionPreview(p);
        Hud_SaveGridSelection(q);
        InventoryPanel_ShowStatusIcons(p);
        MenuDisplay_UpdateActionPrompt(p, 4);
        InventoryPanel_DrawStatComparison(p, 0, 0, 0, 1, 0);
        MenuDisplay_SetPaletteOverrides(p, 1, 0, 0, 0);
    }
}

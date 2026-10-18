#include "global.h"
#include "scene_hud_display.h"
void PauseMenu_SmoothPoint(void *, void *, s32);
void PauseMenu_InterpolateActivePairPositions(void *, s32);
void sub_0822EB14(u32, u32);
void sub_0822E764(u32, u32);
void sub_08172638(void *);
void SceneHud_UpdateActionChoice(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void MenuDisplay_SetPaletteOverrides(void *, int, int, int, int);
void SceneHud_ConfigureActionGrid(void *, u32, u32);
void SceneHud_ShowSelectionDescription(void *);
void SceneHud_FinishActionTransfer(struct SceneHudState *p)
{
    u16 *timer = &p->actionTransferDelay;
    if (!*timer)
        Sound_PlaySong(0x38a);
    (*timer)++;
    if (*timer <= 11) {
        s32 n = 12 - *timer;
        PauseMenu_SmoothPoint(&p->motionWidgets[0].position, &p->actionTargets[0], n);
        PauseMenu_SmoothPoint(&p->motionWidgets[1].position, &p->actionTargets[1], n);
        PauseMenu_InterpolateActivePairPositions(p, n);
    } else {
        u32 *q = &p->motionWidgets[0].flags;
        u32 v = *q, m = 1;
        *q = v | m;
        p->motionWidgets[1].flags |= m;
        p->transferWidgets[0].flags |= m;
        p->transferWidgets[1].flags |= m;
        if (p->actionInput[1] <= 1)
            sub_0822EB14(p->actionSelection, p->actionCursor);
        else
            sub_0822E764(p->actionSelection - 8, p->actionCursor - 8);
        sub_08172638(p);
        p->categoryWidgets[6].flags |= 1;
        Hud_SetInteractionCallback(p, SceneHud_UpdateActionChoice, 1);
        MenuDisplay_SetPaletteOverrides(p, 1, 0, 0, 0);
        SceneHud_ConfigureActionGrid(p->actionInput, 0, 0);
        SceneHud_ShowSelectionDescription(p);
    }
}

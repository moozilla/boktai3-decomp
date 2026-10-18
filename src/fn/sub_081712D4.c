#include "global.h"
#include "scene_hud_display.h"
void sub_08170D78(void *);
void sub_081716B4(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void sub_0816CDB8(void *, int, int);
void Hud_SetGridPosition(void *, int, int);
void Hud_SaveGridSelection(void *);
void PauseInventory_RefreshConsumableDescription(void *);
void MenuDisplay_SetPaletteOverrides(void *, int, int, int, int);
s32 Inventory_GetConsumableId(u32);
s32 Player_GetCandidateAttribute(s32, s32);
s32 Player_GetCurrentAttribute(s32);
void SceneHud_DrawComparisonBars(void *, s32, s32, s16 *);
void sub_0816701C(void *, int, int, int, int, int);
void MenuDisplay_UpdateActionPrompt(void *, int);
void SceneHud_OpenStatComparison(struct SceneHudState *p, s32 reset)
{
    s16 differences[3];
    s16 *end;
    s32 zero = 0;
    s32 i;
    void *input;
    end = &differences[2];
    do {
        *end-- = zero;
    } while ((s32)end >= (s32)differences);
    sub_08170D78(p);
    Hud_SetInteractionCallback(p, sub_081716B4, 1);
    input = p->actionInput;
    sub_0816CDB8(input, 1, 0);
    if (reset)
        Hud_SetGridPosition(input, 0, 0);
    Hud_SaveGridSelection(input);
    PauseInventory_RefreshConsumableDescription(p);
    Hud_SaveGridSelection(input);
    MenuDisplay_SetPaletteOverrides(p, 1, 0, 0, 0);
    for (i = 0; i < 3; i++) {
        s32 value = Player_GetCandidateAttribute(i, Inventory_GetConsumableId(p->actionCursor));
        differences[i] = value - Player_GetCurrentAttribute(i);
    }
    SceneHud_DrawComparisonBars(p, 10, 6, differences);
    sub_0816701C(p, 0, 0, 1, 0, differences[2]);
    MenuDisplay_UpdateActionPrompt(p, 3);
}

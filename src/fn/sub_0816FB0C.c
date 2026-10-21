#define gSceneHudActionDefinitions gUnk_08E61280
#include "global.h"
#include "scene_hud_display.h"
#include "scene_item_values.h"
extern const struct SceneValueItemDefinition gUnk_08E61280[];
void Sound_PlaySong(u32);
s32 Inventory_GetQuantityByStorage(u32, u32);
void InventoryGrid_BeginIconTransfer(void *);
void MenuTransition_BeginSelectedWidget(void *);
void sub_0816E570(void *);
void PauseMenu_BeginCharacterItemUse(void *, u32);
void PauseMenu_BeginVitalRatioExchange(void *, u32);
void ScenePanel_BeginRandomTransition(void *);
void PauseMenu_BeginTypeThirteenItemUse(void *);
void PauseMenu_BeginTypeFourteenItemUse(void *);
void SceneHud_BeginSelectedActionTransfer(void *);
void sub_0816D634(void *, u32);
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
void SceneHud_DispatchSelectedAction(struct SceneHudState *p)
{
    int index = Inventory_GetQuantityByStorage(p->actionCategory, p->actionSelection);
    void *q;
    switch (gUnk_08E61280[index].actionKind) {
        case 1:
        case 2:
        case 3:
        case 5:
        case 9:
            InventoryGrid_BeginIconTransfer(p);
            break;
        case 4:
            Sound_PlaySong(0x38A);
            MenuTransition_BeginSelectedWidget(p);
            break;
        case 7:
            Sound_PlaySong(0x10D);
            if (index == 26)
                Sound_PlaySong(0x182);
            goto apply;
        case 8:
            if (gUnk_08E61280[index].unknown6 == 2)
                sub_0816E570(p);
            else
                PauseMenu_BeginCharacterItemUse(p, gUnk_08E61280[index].unknown6);
            break;
        case 10:
            PauseMenu_BeginVitalRatioExchange(p, gUnk_08E61280[index].unknown6);
            break;
        case 11:
            ScenePanel_BeginRandomTransition(p);
            break;
        case 6:
        case 12:
            Sound_PlaySong(0x10D);
        apply:
            sub_0816D634(p, p->actionSelection);
            SceneHud_RefreshActionIcons(p);
            Hud_SetInteractionCallback(p, SceneHud_UpdateActionSelection, 1);
            q = p->actionInput;
            sub_0816CDB8(q, 1, 0);
            SceneHud_GetActionIconPosition((u8 *)p + 0x1620, p->actionCursor);
            SceneHud_ShowActionPreview(p);
            Hud_SaveGridSelection(q);
            InventoryPanel_ShowStatusIcons(p);
            MenuDisplay_UpdateActionPrompt(p, 4);
            InventoryPanel_DrawStatComparison(p, 0, 0, 0, 1, 0);
            MenuDisplay_SetPaletteOverrides(p, 1, 0, 0, 0);
            break;
        case 13:
            Sound_PlaySong(0x38A);
            PauseMenu_BeginTypeThirteenItemUse(p);
            break;
        case 14:
            Sound_PlaySong(0x38A);
            PauseMenu_BeginTypeFourteenItemUse(p);
            break;
        case 16:
            SceneHud_BeginSelectedActionTransfer(p);
            break;
    }
}

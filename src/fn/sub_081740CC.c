#include "global.h"
#include "scene_hud_display.h"
int SceneInput_NavigateGridKeys(void *, int);
void SceneHud_GetActionIconPosition(void *, int);
void SceneHud_ShowEquipmentDetails(void *);
void MenuDisplay_UpdateActionPrompt(void *, int);
void Hud_SaveGridSelection(void *);
s32 Inventory_GetEquipmentId(u32);
s32 Inventory_CanAssignEquipment(u32);
s32 Inventory_IsEquipmentAssigned(u32);
void PauseMenu_AddSpriteFlags(void *, u32, u32);
void PauseMenu_OpenPrompt(u32, u32, u32);
void PauseMenu_SetConfirmationIndicator(void *, u32);
void MenuDisplay_SetPaletteOverrides(void *, int, int, int, int);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void MotionSequence_ResolveAlternateSelection(void);
void SceneHud_BeginEquipmentSelection(void *);
void Sound_PlaySong(u32);
void sub_081740CC(struct SceneHudState *p)
{
    u8 *input = p->actionInput;
    u32 keys;
    u32 two;
    if (SceneInput_NavigateGridKeys(input, 0xF0)) {
        SceneHud_GetActionIconPosition(&p->categoryWidgets[5].position, p->actionCursor);
        SceneHud_ShowEquipmentDetails(p);
        MenuDisplay_UpdateActionPrompt(p, 2);
    }
    keys = gPadState[1];
    if (keys & 1) {
        s32 assigned;
        Hud_SaveGridSelection(input);
        if (Inventory_GetEquipmentId(p->actionSelection) == 0xFF ||
            !Inventory_CanAssignEquipment(p->actionSelection) ||
            (assigned = Inventory_IsEquipmentAssigned(p->actionSelection)) != 0) {
            Sound_PlaySong(0x192);
        } else {
            PauseMenu_AddSpriteFlags(p, 0x3B, 1);
            PauseMenu_OpenPrompt(*(u32 *)((u8 *)p + 0x5C), 2, 1);
            PauseMenu_SetConfirmationIndicator(p, 0);
            MenuDisplay_SetPaletteOverrides(p, 0, 0, 1, assigned);
            Hud_SetInteractionCallback(p, MotionSequence_ResolveAlternateSelection, 1);
        }
    } else if ((two = 2, two &= keys)) {
        SceneHud_BeginEquipmentSelection(p);
        Sound_PlaySong(0xDE);
    }
}

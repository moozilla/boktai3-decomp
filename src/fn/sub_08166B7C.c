#define MenuSelection_SaveCurrent      Hud_SaveSecondaryGridSelection
#define MenuDisplay_IsEntryUnavailable PauseMenu_IsCategoryEntryEmpty
#include "global.h"
struct MenuSelection {
    u8 unknown0[3];
    u8 current;
    u8 unknown4[5];
    u8 saved;
    u8 unknownA[2];
};
struct MenuDisplay {
    u8 unknown0[0x4384];
    struct MenuSelection selection;
    u8 unknown4390[0x4D0];
    u8 page;
};
s32 SceneHud_SetPairedIcons(void *, s32, s32);
s32 PauseMenu_IsCategoryEntryEmpty(void *, u32, u32);
s32 PauseMenu_GetCategoryEntryValue(u32, s32);
void Hud_SaveSecondaryGridSelection(void *);
s32 Inventory_GetQuantityByStorage(u32, u32);
s32 MenuEntries_CountAvailable(void *, s32);
s32 sub_0816D4E4(s32);
s32 sub_0822DCB4(u32);
s32 MenuDisplay_UpdateActionPrompt(struct MenuDisplay *p, s32 group)
{
    struct MenuSelection *selection = &p->selection;
    u32 action = 0;
    s32 special, result;
    if ((u8)(selection->current - 0x20) <= 8) {
        Hud_SaveSecondaryGridSelection(selection);
        special = 1;
    } else
        special = 0;
    switch (selection->saved) {
        case 0x20:
            if (special == 1 || PauseMenu_IsCategoryEntryEmpty(p, selection->current, group)) {
                action = 0;
            } else if (group == 4) {
                if (sub_0816D4E4(Inventory_GetQuantityByStorage(p->page, selection->current)))
                    action = 1;
            } else if (PauseMenu_GetCategoryEntryValue(selection->current, group)) {
                if (MenuEntries_CountAvailable(p, group) > 1 || group == 3)
                    action = 4;
            } else {
                if (group != 2 || sub_0822DCB4(selection->current)) {
                    if (MenuEntries_CountAvailable(p, group) <= 7)
                        action = 1;
                }
            }
            break;
        case 0x26:
            if (!special)
                action = 3;
            break;
        case 0x28:
            action = 2;
            break;
        case 0x27:
            action = 5;
            break;
    }
    switch (action) {
        case 1:
            if (group == 4)
                result = SceneHud_SetPairedIcons(p, 1, 2);
            else
                result = SceneHud_SetPairedIcons(p, 1, 6);
            break;
        case 4:
            result = SceneHud_SetPairedIcons(p, 1, 3);
            break;
        case 3:
            result = SceneHud_SetPairedIcons(p, 1, 4);
            break;
        case 0:
        case 2:
            result = SceneHud_SetPairedIcons(p, 1, -1);
            break;
        case 5:
            result = SceneHud_SetPairedIcons(p, 1, 5);
            break;
        default:
            result = SceneHud_SetPairedIcons(p, -1, -1);
    }
    return result;
}

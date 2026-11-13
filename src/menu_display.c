#include "menu_display.h"

s32 Inventory_GetEquipmentId(u32);

s32 Inventory_GetQuantityByStorage(u32, u32);

s32 GameState_HasUnlockedInventoryEntry(u32);

s32 SolarFruit_IsInventorySlotEquipped(u32);

s32 Inventory_IsEquipmentAssigned(u32);

s32 GameState_IsConsumableSlotEquipped(u32);

void SceneHud_SetActionCursorPosition(void *, s32);

void SceneHud_GetActionIconPosition(void *, s32);

void MenuDisplay_SetPaletteOverrides(struct MenuDisplay *hud, s32 primary, s32 secondary,
                                     s32 alternate, s32 comparison)
{
    if (primary == 1) {
        hud->hud.categoryWidgets[5].paletteBank = 0xB100;
        hud->hud.categoryWidgets[5].palette = hud->hud.overridePalette;
    } else
        Anim_SelectPaletteBank(&hud->hud.categoryWidgets[5], 0x3AA);
    if (secondary == 1) {
        hud->hud.categoryWidgets[4].paletteBank = 0xB100;
        hud->hud.categoryWidgets[4].palette = hud->hud.overridePalette;
    } else
        Anim_SelectPaletteBank(&hud->hud.categoryWidgets[4], 0x3AA);
    if (alternate == 1) {
        hud->hud.categoryWidgets[7].paletteBank = 0xB100;
        hud->hud.categoryWidgets[7].palette = hud->hud.overridePalette;
    } else
        Anim_SelectPaletteBank(&hud->hud.categoryWidgets[7], 0x3AA);
    if (comparison == 1) {
        hud->hud.comparisonIndicators[0].paletteBank = 0xB100;
        hud->hud.comparisonIndicators[0].palette = hud->hud.overridePalette;
    } else
        Anim_SelectPaletteBank(&hud->hud.comparisonIndicators[0], 0x3AA);
}

void MenuDisplay_UpdateHighlightPalette(struct MenuDisplay *hud)
{
    Palette_UpdateHighlightPulse(&hud->hud.overridePalette[12], &hud->hud.highlightPalettePhase);
}

void MenuDisplay_LoadAdjustedDigits(s32 value, s32 adjustment, void *data, void *hundredsAnim,
                                    void *tensAnim, void *onesAnim)
{
    s32 hundreds, tens, ones, bank;
    value += adjustment;
    hundreds = Div(value, 100);
    value -= hundreds * 100;
    tens = Div(value, 10);
    value -= tens * 10;
    ones = value;
    bank = 10;
    if (adjustment <= 0) {
        bank = 0;
        if (adjustment < 0)
            bank = 20;
    }
    Anim_LoadKeyframe(hundredsAnim, data, (u16)(bank + hundreds), 1);
    Anim_LoadKeyframe(tensAnim, data, (u16)(bank + tens), 1);
    Anim_LoadKeyframe(onesAnim, data, (u16)(bank + ones), 1);
}

static inline void SetPos(struct ContactPosition *position, s32 x, s32 y)
{
    position->x = x;
    position->y = y;
    position->z = 0;
}

static inline void Show(struct SceneInputWidget *widget, s32 *mask)
{
    u32 flags = widget->flags;
    *mask = ~1;
    widget->flags = flags & *mask;
}

void MenuDisplay_DrawStatComparison(struct MenuDisplay *hud, s32 col, s32 row)
{
    s32 i;
    s32 j;
    s32 showMask;
    u16 *cell;
    for (i = 0; i <= 2; i++) {
        showMask = ~1;
        hud->hud.comparisonValues[i] = hud->hud.actionOwner->derivedStats[i];
        if (hud->hud.comparisonValues[i] > 100)
            hud->hud.comparisonValues[i] = 100;
        cell = Hud_GetTilemapCell(0, col, row + i);
        for (j = 0; j < 3; j++)
            *(cell++) = 0xF001;

        MenuDisplay_LoadAdjustedDigits(hud->hud.comparisonValues[i] + hud->hud.comparisonDeltas[i],
                                       hud->hud.comparisonOffsets[i], hud->hud.widgetResource,
                                       &hud->hud.motionWidgets[2 + (i * 3) + 0],
                                       &hud->hud.motionWidgets[2 + (i * 3) + 1],
                                       &hud->hud.motionWidgets[2 + (i * 3) + 2]);
        SetPos(&hud->hud.motionWidgets[2 + (i * 3) + 0].position, col * 8, (row + i) * 8);
        SetPos(&hud->hud.motionWidgets[2 + (i * 3) + 1].position, (col * 8) + 8, (row + i) * 8);
        SetPos(&hud->hud.motionWidgets[2 + (i * 3) + 2].position, (col * 8) + 16, (row + i) * 8);
        {
            /* Advance one row of three widget slots in the shared HUD layout. */
            struct MenuDisplay *rowLayout =
                (struct MenuDisplay *)(i * 3 * sizeof(struct SceneInputWidget) + (u32)hud);
            Show(&rowLayout->hud.motionWidgets[2], &showMask);
            Show(&rowLayout->hud.motionWidgets[3], &showMask);
            Show(&rowLayout->hud.motionWidgets[4], &showMask);
        }
    }
}

s32 MenuDisplay_FindEntryIndex(struct MenuDisplay *hud, s32 entry, s32 group)
{
    s32 i;
    switch (group) {
        case 0:
            for (i = 0; i <= 7; i++)
                if (hud->hud.categoryEntrySlots[i] >= 0 && hud->hud.categoryEntrySlots[i] == entry)
                    return i;
            break;
        case 1:
            for (i = 0; i <= 7; i++)
                if (hud->hud.unlockEntrySlots[i] >= 0 && hud->hud.unlockEntrySlots[i] == entry)
                    return i;
            break;
        case 2:
            for (i = 0; i <= 7; i++)
                if (hud->hud.equippedIconSlots[i] >= 0 && hud->hud.equippedIconSlots[i] == entry)
                    return i;
            break;
        case 3:
            for (i = 0; i <= 3; i++)
                if (hud->consumableEntrySlots[i] >= 0 && hud->consumableEntrySlots[i] == entry)
                    return i;
            break;
    }
    return -1;
}

void MenuDisplay_InterpolateActivePairPositions(struct MenuDisplay *hud, s32 weight)
{
    if (hud->activePairIndices[0] >= 0)
        PauseMenu_SmoothPoint(&hud->hud.transferWidgets[0].position.x, &hud->hud.actionTargets[0].x,
                              weight);
    if (hud->activePairIndices[1] >= 0)
        PauseMenu_SmoothPoint(&hud->hud.transferWidgets[1].position.x, &hud->hud.actionTargets[1].x,
                              weight);
}

void MenuDisplay_BeginActionTransfer(struct MenuDisplay *hud, s32 mode)
{
    if (mode == 0) {
        if ((hud->activePairIndices[0] =
                 MenuDisplay_FindEntryIndex(hud, hud->hud.actionSelection, 0)) >= 0) {
            hud->hud.categoryWidgets[hud->activePairIndices[0] + 0x1E].flags |= 1;
            SceneHud_SetActionCursorPosition(&hud->hud.transferWidgets[0].position,
                                             hud->hud.actionSelection);
        }
        if ((hud->activePairIndices[1] =
                 MenuDisplay_FindEntryIndex(hud, hud->hud.actionCursor, 0)) >= 0) {
            hud->hud.categoryWidgets[hud->activePairIndices[1] + 0x1E].flags |= 1;
            SceneHud_SetActionCursorPosition(&hud->hud.transferWidgets[1].position,
                                             hud->hud.actionCursor);
        }
    } else if (mode == 3) {
        if ((hud->activePairIndices[0] =
                 MenuDisplay_FindEntryIndex(hud, hud->hud.actionSelection, 3)) >= 0) {
            hud->hud.categoryWidgets[hud->activePairIndices[0] + 0x1E].flags |= 1;
            SceneHud_GetActionIconPosition(&hud->hud.transferWidgets[0].position,
                                           hud->hud.actionSelection);
        }
        if ((hud->activePairIndices[1] =
                 MenuDisplay_FindEntryIndex(hud, hud->hud.actionCursor, 3)) >= 0) {
            hud->hud.categoryWidgets[hud->activePairIndices[1] + 0x1E].flags |= 1;
            SceneHud_GetActionIconPosition(&hud->hud.transferWidgets[1].position,
                                           hud->hud.actionCursor);
        }
    } else if (mode == 2) {
        if ((hud->activePairIndices[0] =
                 MenuDisplay_FindEntryIndex(hud, hud->hud.actionSelection, 2)) >= 0) {
            hud->hud.categoryWidgets[hud->activePairIndices[0] + 0x1E].flags |= 1;
            SceneHud_GetActionIconPosition(&hud->hud.transferWidgets[0].position,
                                           hud->hud.actionSelection);
        }
        if ((hud->activePairIndices[1] =
                 MenuDisplay_FindEntryIndex(hud, hud->hud.actionCursor, 2)) >= 0) {
            hud->hud.categoryWidgets[hud->activePairIndices[1] + 0x1E].flags |= 1;
            SceneHud_GetActionIconPosition(&hud->hud.transferWidgets[1].position,
                                           hud->hud.actionCursor);
        }
    } else if (mode == 1) {
        if ((hud->activePairIndices[0] =
                 MenuDisplay_FindEntryIndex(hud, hud->hud.actionSelection - 8, 1)) >= 0) {
            hud->hud.categoryWidgets[hud->activePairIndices[0] + 0x26].flags |= mode;
            SceneHud_SetActionCursorPosition(&hud->hud.transferWidgets[0].position,
                                             hud->hud.actionSelection);
        }
        if ((hud->activePairIndices[1] =
                 MenuDisplay_FindEntryIndex(hud, hud->hud.actionCursor - 8, 1)) >= 0) {
            hud->hud.categoryWidgets[hud->activePairIndices[1] + 0x26].flags |= mode;
            SceneHud_SetActionCursorPosition(&hud->hud.transferWidgets[1].position,
                                             hud->hud.actionCursor);
        }
    }
    if (hud->activePairIndices[0] >= 0)
        hud->hud.transferWidgets[0].flags &= ~1;
    if (hud->activePairIndices[1] >= 0)
        hud->hud.transferWidgets[1].flags &= ~1;
}

void Hud_SaveSecondaryGridSelection(void *);

s32 SceneHud_CanApplySelectedAction(s32);

s32 ItemRecord_MeetsLevelRequirement(u32);

u32 MenuDisplay_IsCategoryEntryEmpty(struct MenuDisplay *menu, u32 entry, u32 category)
{
    u32 empty;
    s32 value;
    switch (category) {
        case 0:
            if (((struct GameState *)(entry * sizeof(s16) + (u32)gGameState))->unk828[0] != -1)
                goto no;
        yes:
            empty = 1;
            goto end;
        case 1:
            if (((struct GameState *)(entry * sizeof(s16) + (u32)gGameState))->table160[424] != 0)
                goto no;
            goto yes;
        case 2:
            if (Inventory_GetEquipmentId(entry) != 0xff)
                goto no;
            goto yes;
        case 3:
            value = ((struct GameState *)(entry * sizeof(s16) + (u32)gGameState))->table160[0];
            goto chk;
        case 4:
            value = Inventory_GetQuantityByStorage(menu->hud.actionCategory, entry);
        chk:
            if (value < 0)
                goto yes;
        default:
        no:
            empty = 0;
    }
end:
    return empty;
}

u32 MenuDisplay_CanSwapCategoryEntries(void *hud, u32 first, u32 second, u32 category)
{
    if (first == second)
        return 0;
    if (MenuDisplay_IsCategoryEntryEmpty(hud, first, category) == 0)
        return 1;
    if (MenuDisplay_IsCategoryEntryEmpty(hud, second, category) == 0)
        return 1;
    return 0;
}

s32 MenuDisplay_GetCategoryEntryValue(u32 entry, s32 category)
{
    switch (category) {
        case 1:
            if (entry - 8 <= 0xb)
                return GameState_HasUnlockedInventoryEntry(entry - 8);
            break;
        case 0:
            if (entry <= 7)
                return SolarFruit_IsInventorySlotEquipped(entry);
            break;
        case 2:
            if (entry <= 0xf)
                return Inventory_IsEquipmentAssigned(entry);
            break;
        case 3:
            if (entry <= 0xf)
                return GameState_IsConsumableSlotEquipped(entry);
            break;
    }
    return 0;
}

s32 MenuDisplay_CountAvailableEntries(void *unused, s32 category)
{
    s32 count = 0;
    s32 i;
    switch (category) {
        case 2:
            for (i = 0; i < 16; i++)
                if (MenuDisplay_GetCategoryEntryValue(i, category))
                    count++;
            break;
        case 1:
            for (i = 8; i < 20; i++)
                if (MenuDisplay_GetCategoryEntryValue(i, category))
                    count++;
            break;
        case 0:
            for (i = 0; i < 8; i++)
                if (MenuDisplay_GetCategoryEntryValue(i, category))
                    count++;
            break;
    }
    return count;
}

/* The legacy caller forwards a register result from the void icon setter. */

typedef s32 (*MenuPromptIconSetter)(void *, s32, s32);

s32 MenuDisplay_UpdateActionPrompt(struct MenuDisplay *menu, s32 group)
{
    struct SceneHudActionGrid *selection = (struct SceneHudActionGrid *)menu->hud.actionInput;
    u32 action = 0;
    s32 special, result;
    if ((u8)(selection->current - 0x20) <= 8) {
        Hud_SaveSecondaryGridSelection(selection);
        special = 1;
    } else
        special = 0;
    switch (selection->saved) {
        case 0x20:
            if (special == 1 || MenuDisplay_IsCategoryEntryEmpty(menu, selection->current, group)) {
                action = 0;
            } else if (group == 4) {
                if (SceneHud_CanApplySelectedAction(Inventory_GetQuantityByStorage(
                        menu->hud.actionCategory, selection->current)))
                    action = 1;
            } else if (MenuDisplay_GetCategoryEntryValue(selection->current, group)) {
                if (MenuDisplay_CountAvailableEntries(menu, group) > 1 || group == 3)
                    action = 4;
            } else {
                if (group != 2 || ItemRecord_MeetsLevelRequirement(selection->current)) {
                    if (MenuDisplay_CountAvailableEntries(menu, group) <= 7)
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
                result = ((MenuPromptIconSetter)PauseMenu_SetPairedIcons)(menu, 1, 2);
            else
                result = ((MenuPromptIconSetter)PauseMenu_SetPairedIcons)(menu, 1, 6);
            break;
        case 4:
            result = ((MenuPromptIconSetter)PauseMenu_SetPairedIcons)(menu, 1, 3);
            break;
        case 3:
            result = ((MenuPromptIconSetter)PauseMenu_SetPairedIcons)(menu, 1, 4);
            break;
        case 0:
        case 2:
            result = ((MenuPromptIconSetter)PauseMenu_SetPairedIcons)(menu, 1, -1);
            break;
        case 5:
            result = ((MenuPromptIconSetter)PauseMenu_SetPairedIcons)(menu, 1, 5);
            break;
        default:
            result = ((MenuPromptIconSetter)PauseMenu_SetPairedIcons)(menu, -1, -1);
    }
    return result;
}

void MenuDisplay_SnapshotValues(struct MenuDisplay *hud)
{
    s32 i;
    for (i = 0; i < 3; i++)
        hud->hud.comparisonDeltas[i] = hud->hud.actionOwner->menuSnapshotValues[i];
}

void MenuDisplay_LoadDecimalDigits(struct MenuDisplay *hud, s32 value, s32 divisor, s32 frameBase,
                                   s32 firstWidget)
{
    s32 i;
    if (value > 999)
        value = 999;
    for (i = 0; divisor > 0; i++) {
        s32 quotient = Div(value, divisor);
        Anim_SelectWidgetGlyph(&hud->hud.categoryWidgets[firstWidget + i], hud->hud.widgetResource,
                               (u16)(frameBase + quotient));
        value -= quotient * divisor;
        divisor = Div(divisor, 10);
    }
}

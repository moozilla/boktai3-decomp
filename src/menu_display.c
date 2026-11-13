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

        MenuDisplay_LoadAdjustedDigits(
            hud->hud.comparisonValues[i] + hud->hud.comparisonDeltas[i], hud->hud.comparisonOffsets[i],
            hud->hud.widgetResource, &hud->hud.motionWidgets[2 + (i * 3) + 0],
            &hud->hud.motionWidgets[2 + (i * 3) + 1], &hud->hud.motionWidgets[2 + (i * 3) + 2]);
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
        if ((hud->activePairIndices[1] = MenuDisplay_FindEntryIndex(hud, hud->hud.actionCursor, 0)) >=
            0) {
            hud->hud.categoryWidgets[hud->activePairIndices[1] + 0x1E].flags |= 1;
            SceneHud_SetActionCursorPosition(&hud->hud.transferWidgets[1].position, hud->hud.actionCursor);
        }
    } else if (mode == 3) {
        if ((hud->activePairIndices[0] =
                 MenuDisplay_FindEntryIndex(hud, hud->hud.actionSelection, 3)) >= 0) {
            hud->hud.categoryWidgets[hud->activePairIndices[0] + 0x1E].flags |= 1;
            SceneHud_GetActionIconPosition(&hud->hud.transferWidgets[0].position, hud->hud.actionSelection);
        }
        if ((hud->activePairIndices[1] = MenuDisplay_FindEntryIndex(hud, hud->hud.actionCursor, 3)) >=
            0) {
            hud->hud.categoryWidgets[hud->activePairIndices[1] + 0x1E].flags |= 1;
            SceneHud_GetActionIconPosition(&hud->hud.transferWidgets[1].position, hud->hud.actionCursor);
        }
    } else if (mode == 2) {
        if ((hud->activePairIndices[0] =
                 MenuDisplay_FindEntryIndex(hud, hud->hud.actionSelection, 2)) >= 0) {
            hud->hud.categoryWidgets[hud->activePairIndices[0] + 0x1E].flags |= 1;
            SceneHud_GetActionIconPosition(&hud->hud.transferWidgets[0].position, hud->hud.actionSelection);
        }
        if ((hud->activePairIndices[1] = MenuDisplay_FindEntryIndex(hud, hud->hud.actionCursor, 2)) >=
            0) {
            hud->hud.categoryWidgets[hud->activePairIndices[1] + 0x1E].flags |= 1;
            SceneHud_GetActionIconPosition(&hud->hud.transferWidgets[1].position, hud->hud.actionCursor);
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
            SceneHud_SetActionCursorPosition(&hud->hud.transferWidgets[1].position, hud->hud.actionCursor);
        }
    }
    if (hud->activePairIndices[0] >= 0)
        hud->hud.transferWidgets[0].flags &= ~1;
    if (hud->activePairIndices[1] >= 0)
        hud->hud.transferWidgets[1].flags &= ~1;
}

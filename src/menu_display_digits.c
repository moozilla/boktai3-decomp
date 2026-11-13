#include "menu_display.h"
s32 Inventory_GetEquipmentId(u32);
s32 Inventory_GetQuantityByStorage(u32, u32);
s32 GameState_HasUnlockedInventoryEntry(u32);
s32 SolarFruit_IsInventorySlotEquipped(u32);
s32 Inventory_IsEquipmentAssigned(u32);
s32 GameState_IsConsumableSlotEquipped(u32);
void SceneHud_SetActionCursorPosition(void *, s32);
void SceneHud_GetActionIconPosition(void *, s32);

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
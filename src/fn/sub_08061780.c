#include "scene_inventory_states.h"
void SceneInventory_BeginSaleConfirmation(struct SceneInventoryControlView *panel)
{
    SceneInput_SaveSelection(&panel->grid);
    panel->interactionFlags = 0x14;
    switch (panel->category) {
        case 0:
            SceneInputPanel_ShowDescription(panel, 1, panel->equipmentText, 0);
            break;
        case 1: {
            const void *message = panel->itemText;
            s32 item = Inventory_GetItemId(panel->grid.savedSelection);
            SceneInputPanel_ShowDescription(panel, 1, message, item);
            break;
        }
        case 2: {
            const void *message = panel->consumableText;
            u32 address = panel->grid.savedSelection;
            struct GameState *game = gGameState;
            s32 item;
            address <<= 1;
            address += (u32)game;
            item = ((struct SceneInventoryConsumableSlot *)address)->id;
            SceneInputPanel_ShowDescription(panel, 1, message, item);
            break;
        }
    }
    SceneInventory_UpdateModePrompt(panel, 2);
    SceneInput_BeginChoice(&panel->display.prompt, &panel->display.widgetResource,
                           &panel->choiceValue, 0, &panel->choiceBlink);
    SceneInventory_SetControllerCallback(
        panel, (SceneInventoryCallback)SceneInventory_TickSaleConfirmation);
}

#include "scene_inventory_states.h"
void SceneInventory_BeginPurchaseConfirmation(struct SceneInventoryControlView *panel)
{
    SceneInput_SaveSelection(&panel->grid);
    panel->interactionFlags = 0x14;
    switch (panel->category) {
        case 0:
            SceneInputPanel_ShowDescription(panel, 0, panel->equipmentText, 0);
            break;
        case 1: {
            const void *message = panel->itemText;
            u8 *saved = &panel->grid.savedSelection;
            s32 index = panel->page * 8 - 16;
            index += *saved;
            SceneInputPanel_ShowDescription(
                panel, 0, message, ((struct SceneInventoryCatalogView *)panel)->entries[index]);
            break;
        }
        case 2: {
            const void *message = panel->consumableText;
            u8 *saved = &panel->grid.savedSelection;
            s32 index = panel->page * 8 - 16;
            index += *saved;
            SceneInputPanel_ShowDescription(
                panel, 0, message, ((struct SceneInventoryCatalogView *)panel)->entries[index]);
            break;
        }
    }
    SceneInventory_UpdateModePrompt(panel, 2);
    SceneInput_BeginChoice(&panel->display.prompt, &panel->display.widgetResource,
                           &panel->choiceValue, 1, &panel->choiceBlink);
    SceneInventory_SetControllerCallback(
        panel, (SceneInventoryCallback)SceneInventory_TickPurchaseConfirmation);
}

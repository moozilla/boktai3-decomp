#include "scene_inventory_states.h"
void SceneInventory_BeginExitConfirmation(struct SceneInventoryControlView *panel)
{
    panel->interactionFlags = 0;
    switch (panel->category) {
        case 0:
            SceneInputPanel_ShowDescription(panel, 3, panel->equipmentText, -1);
            break;
        case 1:
            SceneInputPanel_ShowDescription(panel, 3, panel->itemText, -1);
            break;
        case 2:
            SceneInputPanel_ShowDescription(panel, 3, panel->itemText, -1);
            break;
    }
    SceneInventory_UpdateModePrompt(panel, 3);
    SceneInput_BeginChoice(&panel->display.prompt, &panel->display.widgetResource,
                           &panel->choiceValue, 1, &panel->choiceBlink);
    SceneInventory_SetControllerCallback(
        panel, (SceneInventoryCallback)SceneInventory_TickResetConfirmation);
}

#include "ui/town/shop_states.h"
void Shop_BeginExitConfirmation(struct SceneInventoryControlView *panel)
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
    Shop_UpdateModePrompt(panel, 3);
    SceneInput_BeginChoice(&panel->display.prompt, &panel->display.widgetResource,
                           &panel->choiceValue, 1, &panel->choiceBlink);
    Shop_SetControllerCallback(
        panel, (SceneInventoryCallback)Shop_TickResetConfirmation);
}

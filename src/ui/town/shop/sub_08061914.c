#include "ui/town/shop_states.h"
void Shop_BeginRemovalConfirmation(struct SceneInventoryControlView *panel)
{
    SceneInput_SaveSelection(&panel->grid);
    panel->interactionFlags = 4;
    switch (panel->category) {
        case 0:
            SceneInputPanel_ShowDescription(panel, 2, panel->equipmentText, 0);
            break;
        case 1:
            SceneInputPanel_ShowDescription(panel, 2, panel->itemText, -1);
            break;
        case 2:
            SceneInputPanel_ShowDescription(panel, 2, panel->itemText, -1);
            break;
    }
    Shop_UpdateModePrompt(panel, 2);
    SceneInput_BeginChoice(&panel->display.prompt, &panel->display.widgetResource,
                           &panel->choiceValue, 0, &panel->choiceBlink);
    Shop_SetControllerCallback(
        panel, (SceneInventoryCallback)Shop_TickDiscardConfirmation);
}

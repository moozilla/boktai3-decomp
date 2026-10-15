#include "scene_inventory_states.h"
#define SceneInventory_BeginRemovalConfirmation sub_08061914
void SceneInventory_BeginRemovalConfirmation(struct SceneInventoryControlView *panel)
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
    sub_080613BC(panel, 2);
    SceneInput_BeginChoice(&panel->display.prompt, &panel->display.widgetResource,
                           &panel->choiceValue, 0, &panel->choiceBlink);
    sub_0806130C(panel, (SceneInventoryCallback)sub_080627C4);
}

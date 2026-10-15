#include "scene_inventory_states.h"
#define SceneInventory_BeginExitConfirmation sub_0806186C
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
    sub_080613BC(panel, 3);
    SceneInput_BeginChoice(&panel->display.prompt, &panel->display.widgetResource,
                           &panel->choiceValue, 1, &panel->choiceBlink);
    sub_0806130C(panel, (void (*)(struct SceneInventoryControlView *))sub_08062298);
}

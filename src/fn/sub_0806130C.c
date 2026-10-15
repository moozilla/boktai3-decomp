#include "scene_inventory_control.h"
#define SceneInventory_SetControllerCallback sub_0806130C
void SceneInventory_SetControllerCallback(struct SceneInventoryControlView *panel,
                                          void (*callback)(struct SceneInventoryControlView *))
{
    struct SceneInventoryCallbackView *view = (void *)panel;
    view->callback = callback;
    view->frame = 0xFFFF;
}

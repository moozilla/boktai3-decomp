#include "scene_inventory_control.h"
#define SceneInventory_SetControllerCallback sub_0806130C
void SceneInventory_SetControllerCallback(void *owner, SceneInventoryCallback callback)
{
    struct SceneInventoryCallbackView *view = owner;
    view->callback = callback;
    view->frame = 0xFFFF;
}

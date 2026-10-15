#include "scene_inventory_control.h"
void SceneInventory_SetControllerCallback(void *owner, SceneInventoryCallback callback)
{
    struct SceneInventoryCallbackView *view = owner;
    view->callback = callback;
    view->frame = 0xFFFF;
}

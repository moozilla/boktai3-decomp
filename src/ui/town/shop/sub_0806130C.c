#include "ui/town/shop_control.h"
void Shop_SetControllerCallback(void *owner, SceneInventoryCallback callback)
{
    struct SceneInventoryCallbackView *view = owner;
    view->callback = callback;
    view->frame = 0xFFFF;
}

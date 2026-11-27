#include "ui/town/shop_control.h"
void Shop_SetControllerCallback(void *owner, ShopCallback callback)
{
    struct ShopCallbackView *view = owner;
    view->callback = callback;
    view->frame = 0xFFFF;
}

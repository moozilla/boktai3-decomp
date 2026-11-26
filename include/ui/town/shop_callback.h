#ifndef GUARD_SHOP_CALLBACK_H
#define GUARD_SHOP_CALLBACK_H
/* Storage/interworking boundary: concrete handlers retain their own return type. */
typedef void (*ShopCallback)(void);
void Shop_SetControllerCallback(void *, ShopCallback);
#endif

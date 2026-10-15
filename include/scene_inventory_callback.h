#ifndef GUARD_SCENE_INVENTORY_CALLBACK_H
#define GUARD_SCENE_INVENTORY_CALLBACK_H
/* Storage/interworking boundary: concrete handlers retain their own return type. */
typedef void (*SceneInventoryCallback)(void);
void SceneInventory_SetControllerCallback(void *, SceneInventoryCallback);
#endif

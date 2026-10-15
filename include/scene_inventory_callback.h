#ifndef GUARD_SCENE_INVENTORY_CALLBACK_H
#define GUARD_SCENE_INVENTORY_CALLBACK_H
/* Storage/interworking boundary: concrete handlers retain their own return type. */
typedef void (*SceneInventoryCallback)(void);
void sub_0806130C(void *, SceneInventoryCallback);
#endif

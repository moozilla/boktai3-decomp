#include "scene_inventory_lifecycle.h"
#define SceneInventory_ReadCompletionScript sub_08063194
void SceneInventory_ReadCompletionScript(struct SceneInventoryLifecycle *p)
{
    s32 result = Script_SeekToKeyword(0x65);
    if (result)
        p->completionScript = Script_GetValue();
    else
        p->completionScript = result;
}

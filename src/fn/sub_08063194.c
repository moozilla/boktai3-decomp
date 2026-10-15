#include "scene_inventory_lifecycle.h"
void SceneInventory_ReadCompletionScript(struct SceneInventoryLifecycle *p)
{
    s32 result = Script_SeekToKeyword(0x65);
    if (result)
        p->completionScript = Script_GetValue();
    else
        p->completionScript = result;
}

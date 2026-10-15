#include "scene_inventory_lifecycle.h"
void SceneInventory_UpdateDisplay(struct SceneInventoryTail *);
void sub_082279A8(s32, s32, s32, s32, s32, s32, s32);
void SceneInputPanel_RequestAction(struct SceneInventoryTail *,
                                   s32 (*)(struct SceneInventoryTail *));
s32 SceneInventory_FinishAndCallScript(struct SceneInventoryTail *);
s32 SceneInventory_RunController(struct SceneInventoryTail *p)
{
    if (sub_0824923C(p, ((struct SceneInventoryLifecycle *)p)->callback) == 0) {
        SceneInventory_UpdateDisplay(p);
    } else {
        Sound_PlaySong(0x119);
        sub_082279A8(3, 5, 4, 4, 4, 0xFFFF, 0);
        SceneInputPanel_RequestAction(p, SceneInventory_FinishAndCallScript);
    }
    return 0;
}

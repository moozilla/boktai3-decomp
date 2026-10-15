#include "scene_inventory_lifecycle.h"
#define SceneInventory_RunController sub_080631E8
void sub_080629E0(struct SceneInventoryTail *);
void sub_082279A8(s32, s32, s32, s32, s32, s32, s32);
void SceneInputPanel_RequestAction(struct SceneInventoryTail *,
                                   s32 (*)(struct SceneInventoryTail *));
s32 sub_08063248(struct SceneInventoryTail *);
s32 SceneInventory_RunController(struct SceneInventoryTail *p)
{
    if (sub_0824923C(p, ((struct SceneInventoryLifecycle *)p)->callback) == 0) {
        sub_080629E0(p);
    } else {
        Sound_PlaySong(0x119);
        sub_082279A8(3, 5, 4, 4, 4, 0xFFFF, 0);
        SceneInputPanel_RequestAction(p, sub_08063248);
    }
    return 0;
}

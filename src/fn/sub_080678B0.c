#include "scene_actor_control.h"
#define SceneActorControl_Destroy sub_080678B0
extern struct SceneActorControlManager *gUnk_02000128;
u32 SceneActorControl_Destroy(struct SceneActorControlManager *p)
{
    s32 i;
    for (i = 0; i < 8; i++) {
        if (SceneActorControl_IsUsed(p, i)) {
            struct SceneActorControlSlot *slot = (void *)((u8 *)p + (i * 0xAC + 0x3C));
            if (slot->active)
                Sprite_Remove(slot);
            Collider_Remove(&slot->collider);
        }
    }
    return (u32)(gUnk_02000128 = 0);
}

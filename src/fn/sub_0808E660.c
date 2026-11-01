#include "scene_actor_chase.h"
void Slime_RollMoveFlag(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        u16 *scale = &actor->field2C2;
        *scale = 0x40;
        Anim_SetDurationScale((u8 *)actor->drawable + 0x48, 0x40);
        SceneActorAnimation_SelectProfileEntry((void *)actor, 0x10);
    }
    {
        u32 *cur = &gRandomIndex;
        u32 index;
        s32 roll;
        index = (*cur + 1) & 0x3ff;
        *cur = index;
        roll = *(u8 *)(index * 2 + RANDOM_TABLE_ADDR);
        if (roll <= 7) {
            u32 mask = 0x10;
            u16 *flags = &actor->flags2D6;
            mask |= *flags;
            *flags = mask;
        } else if (roll <= 13) {
            u32 mask = 0x20;
            u16 *flags = &actor->flags2D6;
            mask |= *flags;
            *flags = mask;
        }
    }
}

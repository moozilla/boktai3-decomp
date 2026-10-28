// MATCHING_CONCESSION: register-pinning
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
        register s32 r asm("r0");
        u32 idx;
        register u32 *g asm("r0") = &gRandomIndex;
        idx = (*g + 1) & 1023;
        *g = idx;
        r = *(u8 *)(idx * 2 + (u32)gRandomTable);
        if (r <= 7)
            r = 0x10;
        else if (r <= 13)
            r = 0x20;
        else
            return;
        {
            register u16 *f asm("r2") = &actor->flags2D6;
            *f = r | *f;
        }
    }
}

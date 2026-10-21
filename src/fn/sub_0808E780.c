// MATCHING_CONCESSION: register-pinning
#include "scene_actor_chase.h"
// Open: `movs r2, #0` is scheduled after the first address computation in the original.
void sub_0808E780(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectProfileEntry((void *)actor, 0x12);
        actor->field2C2 = 0x40;
        Anim_SetDurationScale((u8 *)actor->drawable + 0x48, 0x40);
    }
    if (actor->field2B3) {
        u8 *q = &actor->state2A8;
        u32 z;
        asm("" : "+r"(q));
        z = 0;
        *q = z;
        actor->phase2A9 = z;
        actor->behavior2AA = z;
        actor->counter2AC = z;
        actor->field2B1 = 1;
        Chase_SetCtl(actor, SceneActorControl_OnIdle, 0x10);
    }
}

#include "scene_actor_chase.h"
#ifdef NONMATCHING
// fuzzy: 98.9%
// Open: `movs r2, #0` is scheduled after the first address computation in the original.
void sub_0808E780(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectProfileEntry((void *)actor, 0x12);
        actor->field2C2 = 0x40;
        Anim_SetDurationScale((u8 *)actor->drawable + 0x48, 0x40);
    }
    if (actor->field2B3) {
        Chase_SetState(actor, 0, 0, 0);
        Chase_SetCtl(actor, SceneActorControl_OnIdle, 0x10);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0808E780);
#endif

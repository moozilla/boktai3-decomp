#include "scene_actor_chase.h"
void Slime_UpdateRandomWait(void);
void Slime_StartRandomWait(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectProfileEntry((void *)actor, 0x11);
        actor->field2C2 = 0x40;
        Anim_SetDurationScale((u8 *)actor->drawable + 0x48, 0x40);
    }
    if (actor->field2B3)
        Chase_SetCtl(actor, Slime_UpdateRandomWait, 0x22);
}

#include "scene_actor_chase.h"
void Slime_RollMoveFlag(void);
void Slime_StartWalkChoice(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectProfileEntry((void *)actor, 1);
        actor->field2C2 = 0x40;
        Anim_SetDurationScale((u8 *)actor->drawable + 0x48, 0x40);
    }
    if (actor->field2B3) {
        s32 r;
        gRandomIndex = (gRandomIndex + 1) & 1023;
        r = *(u16 *)(gRandomIndex * 2 + (u32)gRandomTable);
        r &= 31;
        if (r <= 23)
            Chase_SetCtl(actor, Slime_RollMoveFlag, 0);
        else
            Chase_SetCtl(actor, Slime_StartWalkChoice, 0x29);
    }
}

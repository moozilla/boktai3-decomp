#include "scene/actor/scene_actors.h"
#include "scene/motion/scene_actor_charge_offset.h"

void SceneActorAnimation_UpdateRecoilSlide(struct StateActor *p, int tick)
{
    u8 *position = (u8 *)p + 0x48;
    if (p->actionChanged)
        p->actionChanged = 0;
    SceneActorState_SetFlags(p, 0x40000);
    if (!p->actionTryUpdate(p)) {
        SceneActorState_SetFlags(p, 64);
        if (!p->actionReset(p)) {
            if (tick <= 10) {
                int rate = 10 - tick;
                struct ContactPosition *velocity = (struct ContactPosition *)((u8 *)p + 0x5c);
                u32 angle = position[9];
                int speed;
                speed = rate * 4;
                SceneActor_SetChargeOffset(angle, velocity, speed);
            }
            if (tick > 9)
                p->actionFinished = 1;
        }
    }
}

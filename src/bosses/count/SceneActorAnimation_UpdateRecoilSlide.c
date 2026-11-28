#include "scene/actor/scene_actors.h"
#include "scene/motion/scene_actor_charge_offset.h"
#include "bosses/count/scene_graphics_actor.h"

void SceneActorAnimation_UpdateRecoilSlide(struct StateActor *p, int tick)
{
    struct SceneActorBody *body = &((struct SceneGraphicsActor *)p)->body;
    if (p->actionChanged)
        p->actionChanged = 0;
    SceneActorState_SetFlags(p, 0x40000);
    if (!p->actionTryUpdate(p)) {
        SceneActorState_SetFlags(p, 0x40);
        if (!p->actionReset(p)) {
            if (tick <= 10)
                SceneActor_SetChargeOffset(body->direction, &body->motion, (10 - tick) * 4);
            if (tick > 9)
                p->actionFinished = 1;
        }
    }
}

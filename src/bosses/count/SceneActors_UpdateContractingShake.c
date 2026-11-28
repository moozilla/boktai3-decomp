#include "scene/actor/scene_actors.h"
#include "scene/motion/scene_actor_charge_offset.h"
#include "bosses/count/scene_graphics_actor.h"

void SceneActors_UpdateContractingShake(struct StateActor *p, int time)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    struct SceneActorBody *body = &actor->body;

    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        if (actor->graphics.animationFlags & 1)
            sequence = 5;
        else
            sequence = 43;
        SceneActors_StartDirectionalAnimation(p, &actor->animation, (int)&actor->animationResource,
                                              sequence, 1, 1, 4);
        SceneActorState_SetFlags(p, 16);
        p->targetPosition = *(struct StateActorPos *)&body->position;
    }
    SceneGraphicsActor_ClearPendingDamage(actor);
    SceneActorState_SetFlags(p, 0x1000c);
    if (!p->actionReset(p)) {
        *(struct StateActorPos *)&body->position = p->targetPosition;
        if (time > 31)
            p->actionFinished = 1;
        else
            SceneActor_SetChargeOffset((~time & 3) << 6, &body->motion, (32 - time) << 3);
    }
}

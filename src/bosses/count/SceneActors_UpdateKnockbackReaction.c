#include "scene/actor/scene_actors.h"
#include "scene/motion/scene_actor_charge_offset.h"
#include "bosses/count/scene_graphics_actor.h"

void SceneActorGraphics_SelectResource(void *);

void SceneActors_UpdateKnockbackReaction(struct StateActor *p, int tick)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    struct SceneActorBody *body = &actor->body;
    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        if (actor->graphics.animationFlags & 1)
            sequence = 11;
        else
            sequence = 49;
        SceneActors_StartDirectionalAnimation(p, &actor->animation, (int)&actor->animationResource,
                                              sequence, 2, 1, 4);
    }
    SceneGraphicsActor_ClearPendingDamage(actor);
    SceneActorState_SetFlags(p, 12);
    if (actor->graphics.scaleMin > 23)
        actor->graphics.scaleMin -= 24;
    else
        actor->graphics.scaleMin = 0;
    if (actor->graphics.scaleMax > 23)
        actor->graphics.scaleMax -= 24;
    else
        actor->graphics.scaleMax = 0;
    if (!p->actionReset(p)) {
        if (tick <= 9)
            SceneActor_SetChargeOffset(actor->hitAngle, &body->motion, 24);
        if (tick > 19) {
            p->actionFinished = 1;
            actor->variant = 0;
        }
    }
}

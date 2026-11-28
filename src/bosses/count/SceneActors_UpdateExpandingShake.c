#include "scene/actor/scene_actors.h"
#include "scene/motion/scene_actor_charge_offset.h"
#include "bosses/count/scene_graphics_actor.h"

void SceneActors_UpdateExpandingShake(struct StateActor *p, int time)
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
        p->targetPosition = *(struct StateActorPos *)&body->position;
    }
    SceneGraphicsActor_ClearPendingDamage(actor);
    SceneActorState_SetFlags(p, 0x1000c);
    if (!p->actionReset(p)) {
        *(struct StateActorPos *)&body->position = p->targetPosition;
        {
            int direction = time & 3;
            struct ContactPosition *velocity = &body->motion;
            int angle = direction << 6;
            int speed = time << 3;
            velocity->x = SceneActor_ChargeComponent(gUnk_086149C4[(angle + 64) & 255] * speed);
            velocity->y = 0;
            velocity->z = SceneActor_ChargeComponent(gUnk_086149C4[angle] * speed);
        }
        if (time > 31)
            p->actionFinished = 1;
    }
}

#include "scene/actor/scene_actors.h"
#include "scene/motion/scene_actor_charge_offset.h"
#include "bosses/count/scene_graphics_actor.h"

int Math_Atan2(int, int);
void SceneActorAnimation_UpdateLungeAttack(struct StateActor *p, int tick)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    struct SceneActorBody *body = &actor->body;
    if (!p->locked)
        SceneActors_SetFacing(p, p->requestedAngle);
    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        if (p->locked) {
            struct ContactPosition *position = &p->target;
            struct ContactPosition *snap = &actor->graphics.snapPosition;
            p->savedFacing = Math_Atan2(snap->x - body->position.x, snap->z - position->z);
            SceneActors_SetFacing(p, p->savedFacing);
        } else {
            if (actor->graphics.stepTableValuePrev == 2)
                Sound_PlaySongChecked(889);
            else
                Sound_PlaySongChecked(888);
        }
        if (actor->graphics.animationFlags & 1)
            sequence = 13;
        else
            sequence = 51;
        SceneActors_StartDirectionalAnimation(p, &actor->animation, (int)&actor->animationResource,
                                              sequence, 2, 3, 4);
    } else {
        int sequence;
        if (actor->graphics.animationFlags & 1)
            sequence = 13;
        else
            sequence = 51;
        SceneActors_StartDirectionalAnimation(p, &actor->animation, (int)&actor->animationResource,
                                              sequence, 2, 3, 1);
    }
    if (actor->graphics.scaleMin > 23)
        actor->graphics.scaleMin -= 24;
    else
        actor->graphics.scaleMin = 0;
    if (actor->graphics.scaleMax > 23)
        actor->graphics.scaleMax -= 24;
    else
        actor->graphics.scaleMax = 0;
    if (!p->actionTryUpdate(p)) {
        SceneActorState_SetFlags(p, 0x40);
        if (!p->actionReset(p)) {
            if (tick <= 63)
                SceneActor_SetChargeOffset(body->direction, &body->motion,
                                           -((64 - tick) >> 3));
            if (tick > 63)
                p->actionFinished = 1;
        }
    }
}

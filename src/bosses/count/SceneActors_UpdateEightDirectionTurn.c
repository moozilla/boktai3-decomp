#include "scene/actor/scene_actors.h"
#include "bosses/count/scene_graphics_actor.h"

/* SceneActors_SetFacing with one local per cue, as this handler compiled it. */
static inline void setFacing(struct StateActor *p, int angle)
{
    p->bodyDirection = angle;
    {
        int shifted = (p->bodyDirection + 0x60) >> 6;
        u32 value = 3;
        value &= shifted;
        p->animationFacing4 = value;
    }
    {
        int shifted = (p->bodyDirection + 0x70) >> 5;
        u32 value = 7;
        value &= shifted;
        p->animationFacing8 = value;
    }
}
void SceneActors_UpdateEightDirectionTurn(struct StateActor *p)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    struct SceneActorBody *body = &actor->body;

    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        if (actor->graphics.animationFlags & 1)
            sequence = 0;
        else
            sequence = 38;
        SceneActors_StartEightDirectionAnimation(p, &actor->animation,
                                                 (int)&actor->animationResource, sequence, 1, 1, 4);
    } else {
        int sequence;
        if (actor->graphics.animationFlags & 1)
            sequence = 0;
        else
            sequence = 38;
        SceneActors_StartEightDirectionAnimation(p, &actor->animation,
                                                 (int)&actor->animationResource, sequence, 1, 1, 1);
    }
    SceneGraphicsActor_ClearPendingDamage(actor);
    SceneActorState_SetFlags(p, 12);
    if (!p->actionReset(p)) {
        int delta = (s8)(body->direction - (u8)p->savedFacing);
        if (abs(delta) <= 7) {
            setFacing(p, p->savedFacing);
            p->actionFinished = 1;
        } else {
            int angle;
            if (delta < 0)
                angle = body->direction + 8;
            else
                angle = body->direction - 8;
            setFacing(p, angle);
        }
        actor->graphics.scaleMin = 0;
        actor->graphics.scaleMax = 0;
        actor->variant = 0;
    }
}

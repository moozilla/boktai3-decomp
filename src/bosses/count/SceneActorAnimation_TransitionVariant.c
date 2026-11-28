#include "scene/actor/scene_actors.h"
#include "bosses/count/scene_graphics_actor.h"

/* Animation flag bit 0 selects the alternate sequences. */
static inline int SceneActorGraphics_SelectSequence(u32 flags, int normal, int alternate)
{
    if (flags & 1)
        return alternate;
    return normal;
}

void SceneActorAnimation_TransitionVariant(struct StateActor *p, int tick)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    if (p->actionChanged) {
        p->actionChanged = 0;
        if (!actor->variant) {
            int sequence;
            actor->variant = 1;
            actor->graphics.scaleStep = 32;
            sequence = SceneActorGraphics_SelectSequence(actor->graphics.animationFlags, 45, 7);
            SceneActors_StartDirectionalAnimation(
                p, &actor->animation, (int)&actor->animationResource, sequence, 2, 1, 4);
        } else {
            int sequence;
            actor->variant = 0;
            actor->graphics.scaleStep = -32;
            sequence = SceneActorGraphics_SelectSequence(actor->graphics.animationFlags, 47, 9);
            SceneActors_StartDirectionalAnimation(
                p, &actor->animation, (int)&actor->animationResource, sequence, 2, 1, 4);
        }
    }
    if (p->actionTryUpdate(p))
        return;
    SceneActorState_SetFlags(p, 0x40);
    if (p->actionReset(p))
        return;
    if (!actor->variant) {
        actor->graphics.scaleMin = actor->graphics.scaleMin * 48 >> 6;
        actor->graphics.scaleMax = actor->graphics.scaleMax * 58 >> 6;
    } else {
        actor->graphics.scaleMin = (actor->graphics.scaleMin * 58 + 960 * 6) >> 6;
        actor->graphics.scaleMax = (actor->graphics.scaleMax * 48 + 1440 * 16) >> 6;
    }
    if (actor->variant == 1) {
        int sequence;
        if (tick > 9) {
            SceneActors_StartEightDirectionAnimation(
                p, &actor->animation, (int)&actor->animationResource,
                SceneActorGraphics_SelectSequence(actor->graphics.animationFlags, 38, 0), 2, 1, 4);
        } else {
            if (actor->graphics.scale > 319)
                sequence = SceneActorGraphics_SelectSequence(actor->graphics.animationFlags, 47, 9);
            else
                sequence = SceneActorGraphics_SelectSequence(actor->graphics.animationFlags, 45, 7);
            SceneActors_StartDirectionalAnimation(
                p, &actor->animation, (int)&actor->animationResource, sequence, 2, 1, 1);
        }
    } else if (actor->graphics.scale > 319) {
        SceneActors_StartEightDirectionAnimation(
            p, &actor->animation, (int)&actor->animationResource,
            SceneActorGraphics_SelectSequence(actor->graphics.animationFlags, 38, 0), 2, 1, 4);
    } else {
        int sequence;
        if (actor->graphics.scale > 79)
            sequence = SceneActorGraphics_SelectSequence(actor->graphics.animationFlags, 47, 9);
        else
            sequence = SceneActorGraphics_SelectSequence(actor->graphics.animationFlags, 45, 7);
        SceneActors_StartDirectionalAnimation(p, &actor->animation, (int)&actor->animationResource,
                                              sequence, 2, 1, 1);
    }
    if (tick > 47) {
        if (!actor->variant) {
            actor->graphics.scaleMin = 0;
            actor->graphics.scaleMax = 0;
        } else {
            actor->graphics.scaleMin = 960;
            actor->graphics.scaleMax = 1440;
        }
        p->actionFinished = 1;
    }
}

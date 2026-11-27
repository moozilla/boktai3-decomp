#include "scene/actor/scene_actors.h"
#include "bosses/count/scene_graphics_actor.h"
static inline int choose(u32 mode, int normal, int alternate)
{
    int test = mode & 1;
    int result;
    result = normal;
    if (test)
        result = alternate;
    return result;
}
static inline int chooseMasked(u32 mode, u32 mask, int normal, int alternate)
{
    int test = mode & mask;
    int result;
    result = normal;
    if (test)
        result = alternate;
    return result;
}
static inline void startEight(struct StateActor *p, struct SceneGraphicsActor *actor, u32 mode,
                              int variant)
{
    int sequence = chooseMasked(mode, variant, 38, 0);
    SceneActors_StartEightDirectionAnimation(p, &actor->animation, (int)&actor->animationResource,
                                             sequence, 2, variant, 4);
}
void SceneActorAnimation_TransitionVariant(struct StateActor *p, int tick)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    int variant;
    if (p->actionChanged) {
        p->actionChanged = 0;
        if (!actor->variant) {
            int sequence;
            actor->variant = 1;
            actor->graphics.scaleStep = 32;
            sequence = choose(actor->graphics.animationFlags, 45, 7);
            SceneActors_StartDirectionalAnimation(
                p, &actor->animation, (int)&actor->animationResource, sequence, 2, 1, 4);
        } else {
            int sequence;
            actor->variant = 0;
            actor->graphics.scaleStep = -32;
            sequence = choose(actor->graphics.animationFlags, 47, 9);
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
        actor->graphics.scaleMin = (actor->graphics.scaleMin * 3) >> 2;
        actor->graphics.scaleMax = (actor->graphics.scaleMax * 58) >> 6;
    } else {
        actor->graphics.scaleMin = (actor->graphics.scaleMin * 58 + 5760) >> 6;
        actor->graphics.scaleMax = (actor->graphics.scaleMax * 48 + 23040) >> 6;
    }
    variant = actor->variant;
    if (variant == 1) {
        int sequence;
        if (tick > 9) {
            startEight(p, actor, actor->graphics.animationFlags, variant);
        } else {
            if (actor->graphics.scale > 319)
                sequence = chooseMasked(actor->graphics.animationFlags, variant, 47, 9);
            else
                sequence = chooseMasked(actor->graphics.animationFlags, variant, 45, 7);
            SceneActors_StartDirectionalAnimation(
                p, &actor->animation, (int)&actor->animationResource, sequence, 2, 1, 1);
        }
    } else if (actor->graphics.scale > 319) {
        int sequence;
        startEight(p, actor, actor->graphics.animationFlags, 1);
    } else {
        int sequence;
        if (actor->graphics.scale > 79)
            sequence = choose(actor->graphics.animationFlags, 47, 9);
        else
            sequence = choose(actor->graphics.animationFlags, 45, 7);
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

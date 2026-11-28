#include "scene/actor/scene_actors.h"
#include "bosses/count/scene_graphics_actor.h"

u8 Sprite_BlinkUntilExpired(int *, int, int, int);
void SceneActors_UpdateBlinkReaction(struct StateActor *p, int tick)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    struct SceneActorBody *body = &actor->body;
    int done;

    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        if (actor->graphics.animationFlags & 1)
            sequence = 32;
        else
            sequence = 70;
        SceneActors_StartUnmirroredAnimation(p, &actor->animation, (int)&actor->animationResource,
                                             sequence, 1, 4);
        p->targetPosition = *(struct StateActorPos *)&body->position;
    }
    done = 0;
    if (Sprite_BlinkUntilExpired(&done, tick, 10, 30))
        SceneActorState_SetFlags(p, 16);
    else if (done)
        SceneActorState_SetFlags(p, 16);
    SceneGraphicsActor_ClearPendingDamage(actor);
    SceneActorState_SetFlags(p, 12);
    if (!p->actionReset(p) && tick > 59)
        p->actionFinished = 1;
}

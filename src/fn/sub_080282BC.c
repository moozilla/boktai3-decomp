#include "scene_actors.h"
#include "scene_graphics_actor.h"

u8 Sprite_BlinkUntilExpired(int *, int, int, int);
void SceneActors_UpdateBlinkReaction(struct StateActor *p, int tick)
{
    struct StateActor *p2 = p;
    u8 *position = (u8 *)p + 0x48;
    int done;
    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        {
            int test = ((struct SceneGraphicsActor *)p)->graphics.animationFlags;
            test &= 1;
            sequence = 70;
            if (test)
                sequence = 32;
        }
        SceneActors_StartUnmirroredAnimation(
            p, &((struct SceneGraphicsActor *)p)->animation,
            (int)&((struct SceneGraphicsActor *)p)->animationResource, sequence, 1, 4);
        p->targetPosition = *(struct StateActorPos *)(position + 12);
    }
    done = 0;
    if (Sprite_BlinkUntilExpired(&done, tick, 10, 30)) {
        SceneActorState_SetFlags(p, 16);
    } else if (done) {
        SceneActorState_SetFlags(p, 16);
    }
    SceneGraphicsActor_ClearPendingDamage((struct SceneGraphicsActor *)p2);
    SceneActorState_SetFlags(p, 12);
    if (!p->actionReset(p) && tick > 59)
        p->actionFinished = 1;
}

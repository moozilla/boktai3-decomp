#include "bosses/sabata/scene_volume.h"
#include "scene/actor/scene_actor_components.h"

static inline void SetDirection(struct SceneVolumeActor *actor, int angle)
{
    actor->direction = angle;
    {
        s32 d = (actor->direction + 0x60) >> 6;
        u32 mask = 3;
        mask &= d;
        actor->direction4 = mask;
    }
    {
        s32 d = (actor->direction + 0x70) >> 5;
        u32 mask = 7;
        mask &= d;
        actor->direction8 = mask;
    }
}

void SceneActors_StartEightDirectionAnimation(void *, void *, void *, int, int, int, int);

// confidence=low: turns toward the saved direction 8 steps a frame, finishing within 8.
void sub_0817F330(struct SceneVolumeActor *p)
{
    struct SceneVolumeActor *actor = p;
    struct SceneActorBody *body = (struct SceneActorBody *)((u8 *)p + 0x48);
    if (p->restart) {
        p->restart = 0;
        SceneActors_StartEightDirectionAnimation(p, &p->animation, p->animationState, 0xF4, 1, 1,
                                                 4);
    } else {
        SceneActors_StartEightDirectionAnimation(p, &p->animation, p->animationState, 0xF4, 1, 1,
                                                 1);
    }
    SceneVolume_SetFlags(actor, 0x400004);
    {
        int delta = (s8)(body->direction - (u8)actor->alternateDirection), distance = delta;
        if (distance < 0)
            distance = -distance;
        if (distance <= 7) {
            SetDirection(actor, actor->alternateDirection);
            actor->pending = 1;
        } else {
            int angle;
            if (delta < 0)
                angle = body->direction + 8;
            else
                angle = body->direction - 8;
            SetDirection(actor, angle);
        }
    }
}

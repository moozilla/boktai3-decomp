#include "scene_volume.h"

void SceneActors_StartEightDirectionAnimation(void *, void *, void *, int, int, int, int);
s32 Callback_InvokeActor(void *, void *);

void ActorAnimation_UpdateDirectional(struct SceneVolumeActor *p)
{
    if (p->restart) {
        p->restart = 0;
        SceneActors_StartEightDirectionAnimation(p, &p->animation, p->animationState, 0xF4, 1, 1,
                                                 4);
        SceneVolume_SetFlags(p, 0x400004);
    } else {
        SceneActors_StartEightDirectionAnimation(p, &p->animation, p->animationState, 0xF4, 1, 1,
                                                 1);
    }
    if ((u8)Callback_InvokeActor(p, p->callback) == 0) {
        if (p->special)
            SceneVolume_SetFlags(p, 0x400004);
        else
            SceneVolume_SetFlags(p, 0x40);
    }
}

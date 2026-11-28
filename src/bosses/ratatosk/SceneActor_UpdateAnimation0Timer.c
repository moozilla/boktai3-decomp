#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
void Sound_PlaySong(u32);
s32 Callback_InvokeActor(void *, void *);
void SceneActor_UpdateAnimation0Timer(struct Ratatosk *p, s32 elapsed)
{
    struct Ratatosk *self = p;
    if (p->actionChanged) {
        p->actionChanged = 0;
        Ratatosk_SetFacing(p, p->savedAngle);
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 0, 1, 1, 4);
    } else {
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 0, 1, 1, 1);
    }
    self->scaleX = 0;
    self->scaleZ = 0;
    self->scaleMode = 0;
    Ratatosk_SetFlags(p, 0x100000);
    if (!(u8)Callback_InvokeActor(p, p->actionTryUpdate)) {
        if (elapsed >= self->interval && !p->locked)
            p->actionFinished = 1;
        Ratatosk_SetFlags(p, 0x10);
    }
}

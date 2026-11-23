#define Callback_InvokeWithArgument Callback_InvokeActor
#include "global.h"
#include "ratatosk.h"
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
void Sound_PlaySong(u32);
static inline void setFacing(struct Ratatosk *p, int angle)
{
    int shifted;
    u32 value;
    p->body.direction = angle;
    shifted = p->body.direction + 0x60;
    shifted >>= 6;
    value = 3;
    value &= shifted;
    p->facing = value;
    shifted = p->body.direction + 0x70;
    shifted >>= 5;
    value = 7;
    value &= shifted;
    p->facing8 = value;
}
s32 Callback_InvokeActor(void *, void *);
void SceneActor_UpdateAnimation0Timer(struct Ratatosk *p, s32 elapsed)
{
    struct Ratatosk *self = p;
    if (p->actionChanged) {
        p->actionChanged = 0;
        setFacing(p, p->savedAngle);
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 0, 1, 1, 4);
    } else {
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 0, 1, 1, 1);
    }
    self->scaleX = 0;
    self->scaleZ = 0;
    self->scaleMode = 0;
    {
        Ratatosk_SetFlags(p, 0x100000);
    }
    if (!(u8)Callback_InvokeWithArgument(p, p->actionTryUpdate)) {
        if (elapsed >= self->interval && !p->locked)
            p->actionFinished = 1;
        {
            Ratatosk_SetFlags(p, 0x10);
        }
    }
}

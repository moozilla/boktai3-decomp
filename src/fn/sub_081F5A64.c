#include "global.h"
#include "ratatosk.h"
void SceneActors_StartAnimation(void *, void *, void *, int, int, int);
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
void SceneActor_UpdateAnimation33Timer(struct Ratatosk *p, s32 elapsed)
{
    if (p->actionChanged) {
        p->actionChanged = 0;
        if (p->locked)
            setFacing(p, p->savedAngle);
        SceneActors_StartAnimation(p, p->animationData, p->animation, 33, 1, 4);
    }
    if (elapsed > 16)
        p->actionFinished = 1;
}

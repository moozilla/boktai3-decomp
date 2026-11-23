#include "global.h"
#include "ratatosk.h"

void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
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
void SceneActor_UpdateAnimationDTimer(struct Ratatosk *p, s32 elapsed)
{
    if (p->actionChanged) {
        p->actionChanged = 0;
        setFacing(p, p->requestedAngle);
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 13, 2, 1, 1);
    }
    if (elapsed > 30)
        p->actionFinished = 1;
}

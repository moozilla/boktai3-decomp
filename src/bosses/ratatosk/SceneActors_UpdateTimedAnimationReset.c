#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
static inline void setFacing(struct Ratatosk *p, int angle)
{
    p->body.direction = angle;
    {
        int shifted = (p->body.direction + 0x60) >> 6;
        u32 value = 3;
        value &= shifted;
        p->facing = value;
    }
    {
        int shifted = (p->body.direction + 0x70) >> 5;
        u32 value = 7;
        value &= shifted;
        p->facing8 = value;
    }
}
void SceneActors_UpdateTimedAnimationReset(struct Ratatosk *p, int tick)
{
    if (p->actionChanged) {
        p->actionChanged = 0;
        setFacing(p, p->savedAngle);
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 26, 1, 1, 4);
        p->actionCounter = 0;
        p->scaleX = 0;
        p->scaleZ = 0;
        p->scaleMode = 0;
    }
    if (tick == 35) {
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 17, 0, 4, 4);
        p->actionFinished = 1;
    }
}

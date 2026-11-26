#include "engine/shared/global.h"
#include "bosses/ratatosk/ratatosk.h"
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
void SceneActors_UpdateTimedDirectionalAnimation(struct Ratatosk *p, int tick)
{
    if (p->actionChanged) {
        p->actionChanged = 0;
        p->body.direction = p->requestedAngle;
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
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 13, 1, 1, 4);
    }
    if (tick > 19) {
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 13, 0, 1, 1);
        p->actionFinished = 1;
    }
}

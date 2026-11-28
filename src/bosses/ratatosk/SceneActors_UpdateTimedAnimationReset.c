#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
void SceneActors_UpdateTimedAnimationReset(struct Ratatosk *p, int tick)
{
    if (p->actionChanged) {
        p->actionChanged = 0;
        Ratatosk_SetFacing(p, p->savedAngle);
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

#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
void SceneActors_UpdateTimedDirectionalAnimation(struct Ratatosk *p, int tick)
{
    if (p->actionChanged) {
        p->actionChanged = 0;
        Ratatosk_SetFacing(p, p->requestedAngle);
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 13, 1, 1, 4);
    }
    if (tick > 19) {
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 13, 0, 1, 1);
        p->actionFinished = 1;
    }
}

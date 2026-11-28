#include "global.h"
#include "bosses/ratatosk/ratatosk.h"

void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
void SceneActor_UpdateAnimationDTimer(struct Ratatosk *p, s32 elapsed)
{
    if (p->actionChanged) {
        p->actionChanged = 0;
        Ratatosk_SetFacing(p, p->requestedAngle);
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 13, 2, 1, 1);
    }
    if (elapsed > 30)
        p->actionFinished = 1;
}

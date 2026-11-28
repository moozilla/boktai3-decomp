#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
void SceneActors_StartAnimation(void *, void *, void *, int, int, int);
void SceneActor_UpdateAnimation33Timer(struct Ratatosk *p, s32 elapsed)
{
    if (p->actionChanged) {
        p->actionChanged = 0;
        if (p->locked)
            Ratatosk_SetFacing(p, p->savedAngle);
        SceneActors_StartAnimation(p, p->animationData, p->animation, 33, 1, 4);
    }
    if (elapsed > 16)
        p->actionFinished = 1;
}

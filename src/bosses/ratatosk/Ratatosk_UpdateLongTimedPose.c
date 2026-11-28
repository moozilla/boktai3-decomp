#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
void SceneActors_StartAnimation(void *, void *, void *, int, int, int);
void Ratatosk_UpdateLongTimedPose(struct Ratatosk *p, s32 timer)
{
    if (p->actionChanged) {
        p->actionChanged = 0;
        SceneActors_StartAnimation(p, p->animationData, p->animation, 0x21, 2, 4);
        p->interval = 120;
    }
    if (timer >= p->interval)
        p->actionFinished = 1;
}

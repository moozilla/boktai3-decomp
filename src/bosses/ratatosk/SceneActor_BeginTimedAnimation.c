#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
void SceneActors_StartAnimation(void *, void *, void *, int, int, int);
void SceneActor_BeginTimedAnimation(struct Ratatosk *p, s32 elapsed)
{
    struct Ratatosk *self = p;
    if (p->actionChanged) {
        p->actionChanged = 0;
        if (p->locked) {
            Ratatosk_SetFacing(p, p->savedAngle);
        }
        SceneActors_StartAnimation(p, p->animationData, p->animation, 32, 0, 4);
        p->interval = 60;
    }
    Ratatosk_SetFlags(p, 0x40000);
    if (elapsed >= self->interval)
        p->actionFinished = 1;
}

#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
// confidence=low: selects a reaction animation from the boss reaction mode.
void SceneActors_StartAnimation(void *, void *, void *, int, int, int);
void Ratatosk_UpdateReactionAnimation(struct Ratatosk *p, s32 frame)
{
    struct Ratatosk *q = p;
    if (p->actionChanged) {
        p->actionChanged = 0;
        if (p->locked)
            Ratatosk_SetFacing(p, p->savedAngle);
        if (p->stateParameter == 1)
            SceneActors_StartAnimation(p, p->animationData, p->animation, 0x21, 2, 4);
        else if (p->stateParameter == 0)
            SceneActors_StartAnimation(p, p->animationData, p->animation, 0x21, 1, 4);
        else
            SceneActors_StartAnimation(p, p->animationData, p->animation, 0x22, 2, 4);
    }
    if (frame == 16) {
        if (q->stateParameter == 1)
            p->actionFinished = 1;
        else if (q->stateParameter == 0)
            SceneActors_StartAnimation(p, q->animationData, q->animation, 0x22, 1, 4);
        else
            goto other;
    }
    if (q->stateParameter == 0) {
        if (frame == 51)
            SceneActors_StartAnimation(p, q->animationData, q->animation, 0x23, 1, 4);
        if (frame > 56)
            p->actionFinished = 1;
    } else {
    other:
        if (q->stateParameter == 1) {
            if (frame == 49)
                p->actionFinished = 1;
        } else if (frame == 34)
            p->actionFinished = 1;
    }
}

#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
void Sound_PlaySongChecked(u32);
void SceneActors_StartAnimation(void *, void *, void *, int, int, int);
void Ratatosk_UpdateTimedPose(struct Ratatosk *p, s32 timer)
{
    struct Ratatosk *p2 = p;
    if (p->actionChanged) {
        p->actionChanged = 0;
        if (p->locked)
            Ratatosk_SetFacing(p, p->savedAngle);
        SceneActors_StartAnimation(p, p->animationData, p->animation, 0x20, 1, 4);
        p->interval = 60;
    }
    if (timer >= p2->interval)
        p->actionFinished = 1;
}

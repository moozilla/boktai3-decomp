#include "global.h"
#include "bosses/ratatosk/ratatosk.h"

void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
void Sound_PlaySongChecked(u32);
s32 Callback_InvokeActor(void *, void *);
void Ratatosk_UpdateDirectionalRecovery(struct Ratatosk *p, s32 timer)
{
    if (p->actionChanged) {
        int angle;
        p->actionChanged = 0;
        angle = p->targetAngle + 0x80;
        Ratatosk_SetFacing(p, angle);
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 0x1d, 0, 1, 4);
        Sound_PlaySongChecked(0x55f);
    }
    p->frameDamage = 0;
    p->motionFlag = 0;
    Ratatosk_SetFlags(p, 12);
    if (!(u8)Callback_InvokeActor(p, p->actionReset) && timer > 19)
        p->actionFinished = 1;
}

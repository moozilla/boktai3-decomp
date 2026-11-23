#include "global.h"
#include "ratatosk.h"

void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
void Sound_PlaySongChecked(u32);
s32 Callback_InvokeActor(void *, void *);
void Ratatosk_UpdateDirectionalRecovery(struct Ratatosk *p, s32 timer)
{
    struct Ratatosk *p2 = p;
    if (p->actionChanged) {
        int angle;
        p->actionChanged = 0;
        angle = p->targetAngle + 0x80;
        Ratatosk_SetFacing(p, angle);
        {
            u16 mode = 0;
            SceneActors_StartDirectionalAnimation(p, p2->animationData, p2->animation, 0x1d, mode,
                                                  1, 4);
        }
        Sound_PlaySongChecked(0x55f);
    }
    p2->frameDamage = 0;
    p2->motionFlag = 0;
    {
        u32 value;
        u32 mask = 12;
        value = p->flags | mask;
        p->flags = value;
    }
    if (!(u8)Callback_InvokeActor(p, p->actionReset) && timer > 19)
        p->actionFinished = 1;
}

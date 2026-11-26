#include "engine/shared/global.h"
#include "bosses/ratatosk/ratatosk.h"

/* Ratatosk field and state interpretations are provisional (low confidence). */
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
s32 Callback_InvokeActor(void *, void *);
void Sound_PlaySong(u32);
void Ratatosk_UpdateTimedAttack(struct Ratatosk *p, s32 frame)
{
    Ratatosk_SetFacing(p, p->requestedAngle);
    if (p->actionChanged) {
        p->actionChanged = 0;
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 9, 2, 3, 4);
        Sound_PlaySong(0x55c);
    }
    if (frame == 13)
        Sound_PlaySong(0x565);
    if ((u32)(frame - 13) <= 12 && p->actionPhase != 5)
        Ratatosk_SetFlags(p, 0x20000);
    if (frame > 37)
        p->actionFinished = 1;
    Callback_InvokeActor(p, p->actionTryUpdate);
}

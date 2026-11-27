#include "global.h"
#include "bosses/ratatosk/ratatosk.h"

/* Ratatosk field and state interpretations are provisional (low confidence). */
s32 Mod(s32, s32);
s32 Ratatosk_GetSelectedPointIndex(void);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Sound_PlaySong(u32);
void Sound_StopSong(u32);
s32 Callback_InvokeActor(void *, void *);
void RisingActorManager_BeginRaise(s32);
void SlotController_RequestReset(s32);
void SlotController_SetSlotFlag2(s32);
void Ratatosk_UpdatePointEffectAnimation(struct Ratatosk *p, s32 frame)
{
    if (p->actionChanged) {
        p->actionChanged = 0;
        SceneActors_StartUnmirroredAnimation(p, p->animationData, p->animation, 16, 1, 4);
        p->actionCounter = 0;
        RisingActorManager_BeginRaise(Ratatosk_GetSelectedPointIndex());
        Sound_PlaySong(0x570);
    }
    if (Mod(frame, 300) == 180)
        SlotController_RequestReset(Ratatosk_GetSelectedPointIndex());
    Ratatosk_SetFlags(p, 0x100000);
    if (frame <= 179)
        SlotController_SetSlotFlag2(Ratatosk_GetSelectedPointIndex());
    if (p->timer > 240) {
        p->actionCounter = 0;
        p->actionFinished = 1;
        Sound_StopSong(0x570);
    }
    Callback_InvokeActor(p, p->actionTryUpdate);
}

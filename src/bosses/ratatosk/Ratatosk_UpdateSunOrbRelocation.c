#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "objects/sun_orb/sun_orb_effect.h"
void Ratatosk_GetPlayerOffsetFarthestFromSelf(void *, void *);
s32 RisingActorManager_BeginRaise(s32);
s32 Ratatosk_GetSelectedPointIndex(void);
s32 SunOrb_IsFadingOut(s32);
s32 SlotController_SetSlotFlag2(s32);
s32 SunOrb_GetSlotState(s32);
s32 SlotController_IsSlotIdle(s32);
s32 sub_081FEC38(struct OrbitEffectManager *, s32);
void Ratatosk_GetPlayerOffsetNearestSelf(void *, void *);
void Ratatosk_UpdateSunOrbRelocation(struct Ratatosk *p)
{
    struct Ratatosk *q = p;
    struct OrbitEffectParams config;
    struct ContactPosition far, near;
    if (Ratatosk_ConsumeActionEntry(p) == 1) {
        if (p->hasOrb == 1) {
            Ratatosk_SetAnimation(q, 10);
            p->sequence = 0;
            p->repeat = 0;
            p->waitTimer = 0;
        } else {
            s32 slot = Ratatosk_GetSelectedPointIndex();
            if (slot >= 0) {
                if (!SunOrb_IsFadingOut(slot)) {
                    p->destination = *(struct ContactPosition *)SunOrb_GetPosition(slot);
                    p->duration = 32;
                    Ratatosk_SetAnimation(q, 4);
                    p->sequence = 1;
                } else {
                    p->sequenceDone = 1;
                    return;
                }
            } else {
                p->sequenceDone = 1;
                return;
            }
        }
    }
    switch (p->sequence) {
        case 0:
            if (q->actionFinished) {
                s32 slot = Ratatosk_GetSelectedPointIndex();
                if (slot >= 0) {
                    if (!SunOrb_IsFadingOut(slot)) {
                        p->destination = *(struct ContactPosition *)SunOrb_GetPosition(slot);
                        p->duration = 32;
                        Ratatosk_SetAnimation(q, 4);
                        p->sequence = 1;
                    } else {
                        p->sequenceDone = 1;
                        return;
                    }
                } else {
                    p->sequenceDone = 1;
                    return;
                }
            }
            break;
        case 1:
            SlotController_SetSlotFlag2(Ratatosk_GetSelectedPointIndex());
            if (q->actionFinished) {
                p->sequence = 2;
                p->waitTimer = 0;
            }
            break;
        case 2: {
            s32 slot = Ratatosk_GetSelectedPointIndex();
            if (SunOrb_GetSlotState(slot) > 60) {
                Ratatosk_SetAnimation(q, 13);
                p->sequence = 3;
                p->previousOrb = slot;
                p->selectedPoint = -1;
                RisingActorConfig_Initialize(&config);
                SunOrb_SetTargetSource(&config, 1);
                Ratatosk_GetPlayerOffsetFarthestFromSelf(p, &far);
                slot = SunOrb_CreateAtPosition(&config, (struct OrbitEffectPosition *)&far);
                if (slot >= 0) {
                    p->targets[slot].active = 1;
                    RisingActorManager_BeginRaise(slot);
                    p->selectedPoint = slot;
                }
            } else {
                if (p->waitTimer > 60 || !SlotController_IsSlotIdle(slot)) {
                    p->sequenceDone = 1;
                    return;
                }
            }
            ++p->waitTimer;
            break;
        }
        case 3:
            SlotController_SetSlotFlag2(Ratatosk_GetSelectedPointIndex());
            if (q->actionFinished) {
                sub_081FEC38(p->sunOrbs, p->previousOrb);
                p->body.position = *(struct ContactPosition *)SunOrb_GetPositionFromManager(
                    p->sunOrbs, p->selectedPoint);
                Ratatosk_SetAnimation(q, 14);
                p->sequence = 4;
            }
            break;
        case 4:
            SlotController_SetSlotFlag2(Ratatosk_GetSelectedPointIndex());
            if (q->actionFinished) {
                Ratatosk_GetPlayerOffsetNearestSelf(p, &near);
                p->destination = near;
                Ratatosk_SetAnimation(q, 4);
                p->sequence = 5;
            }
            break;
        case 5:
            if (q->actionFinished)
                p->sequenceDone = 1;
            break;
    }
    ++p->timer;
}

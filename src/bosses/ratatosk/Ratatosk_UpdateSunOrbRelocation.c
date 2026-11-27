#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "objects/sun_orb/sun_orb_effect.h"
#include "bosses/ratatosk/ratatosk_orb_targets.h"
#include "bosses/ratatosk/ratatosk_orb_distance_view.h"
static inline u8 ConsumeActivation(struct RatatoskOrbDistanceView *p)
{
    if (p->active) {
        p->active = 0;
        p->input = 0;
        return 1;
    }
    return 0;
}
void Ratatosk_GetPlayerOffsetFarthestFromSelf(void *, void *);
s32 RisingActorManager_BeginRaise(s32);
s32 Ratatosk_GetSelectedPointIndex(void);
s32 SunOrb_IsFadingOut(s32);
s32 SlotController_SetSlotFlag2(s32);
s32 SunOrb_GetSlotState(s32);
s32 SlotController_IsSlotIdle(s32);
s32 sub_081FEC38(struct OrbitEffectManager *, s32);
void Ratatosk_GetPlayerOffsetNearestSelf(void *, void *);
void Ratatosk_UpdateSunOrbRelocation(struct RatatoskOrbDistanceView *p)
{
    struct RatatoskOrbDistanceView *q = p;
    struct OrbitEffectParams config;
    struct OrbitEffectPosition far, near;
    s32 activated = ConsumeActivation(p);
    if (activated == 1) {
        if (p->orbCreated == 1) {
            u32 animation = 10;
            q->animation = animation;
            p->sequence = 0;
            p->completion = 0;
            p->repeat = 0;
        } else {
            s32 slot = Ratatosk_GetSelectedPointIndex();
            if (slot >= 0) {
                if (!SunOrb_IsFadingOut(slot)) {
                    p->destination = *SunOrb_GetPosition(slot);
                    p->duration = 32;
                    {
                        u32 animation = 4;
                        q->animation = animation;
                    }
                    p->sequence = activated;
                } else {
                    p->finished = activated;
                    return;
                }
            } else {
                p->finished = activated;
                return;
            }
        }
    }
    switch (p->sequence) {
        case 0:
            if (q->input) {
                s32 slot = Ratatosk_GetSelectedPointIndex();
                if (slot >= 0) {
                    if (!SunOrb_IsFadingOut(slot)) {
                        p->destination = *SunOrb_GetPosition(slot);
                        p->duration = 32;
                        {
                            u32 animation = 4;
                            q->animation = animation;
                        }
                        p->sequence = 1;
                    } else {
                        p->finished = 1;
                        return;
                    }
                } else {
                    p->finished = 1;
                    return;
                }
            }
            break;
        case 1:
            SlotController_SetSlotFlag2(Ratatosk_GetSelectedPointIndex());
            if (q->input) {
                p->sequence = 2;
                p->repeat = 0;
            }
            break;
        case 2: {
            s32 slot = Ratatosk_GetSelectedPointIndex();
            if (SunOrb_GetSlotState(slot) > 60) {
                u32 animation = 13;
                q->animation = animation;
                p->sequence = 3;
                p->oldSlot = slot;
                p->orbsStorage.orbs.selection.selectedPoint = -1;
                RisingActorConfig_Initialize(&config);
                SunOrb_SetTargetSource(&config, 1);
                Ratatosk_GetPlayerOffsetFarthestFromSelf(p, &far);
                slot = SunOrb_CreateAtPosition(&config, &far);
                if (slot >= 0) {
                    p->orbsStorage.orbs.targets[slot].active = 1;
                    RisingActorManager_BeginRaise(slot);
                    p->orbsStorage.orbs.selection.selectedPoint = slot;
                }
            } else {
                if (p->repeat > 60 || !SlotController_IsSlotIdle(slot)) {
                    p->finished = 1;
                    return;
                }
            }
            ++p->repeat;
            break;
        }
        case 3:
            SlotController_SetSlotFlag2(Ratatosk_GetSelectedPointIndex());
            if (q->input) {
                struct OrbitEffectManager **manager = &p->manager;
                sub_081FEC38(*manager, p->oldSlot);
                p->position = *SunOrb_GetPositionFromManager(
                    *manager, p->orbsStorage.orbs.selection.selectedPoint);
                {
                    u32 animation = 14;
                    q->animation = animation;
                }
                p->sequence = 4;
            }
            break;
        case 4:
            SlotController_SetSlotFlag2(Ratatosk_GetSelectedPointIndex());
            if (q->input) {
                Ratatosk_GetPlayerOffsetNearestSelf(p, &near);
                p->destination = near;
                {
                    u32 animation = 4;
                    q->animation = animation;
                }
                p->sequence = 5;
            }
            break;
        case 5:
            if (q->input)
                p->finished = 1;
            break;
    }
    ++p->elapsed;
}

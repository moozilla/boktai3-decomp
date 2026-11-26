#include "engine/shared/global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "engine/shared/engine.h"
#include "engine/shared/ram.h"

struct OrbConfig {
    u32 words[8];
};
s32 Ratatosk_GetSelectedPointIndex(void);
s32 SunOrb_IsFadingOut(void);
struct ContactPosition *SunOrb_GetPosition(s32);
void SlotController_SetSlotFlag2(s32);
s32 SunOrb_GetSlotState(s32);
void RisingActorConfig_Initialize(void *);
void SunOrb_SetTargetSource(void *, s32);
void Ratatosk_GetPlayerOffsetFarthestFromSelf(void *, void *);
void Ratatosk_GetPlayerOffsetNearestSelf(void *, void *);
s32 SunOrb_CreateAtPosition(void *, void *);
void RisingActorManager_BeginRaise(s32);
void sub_081FEC38(s32, s32);
struct ContactPosition *SunOrb_GetPositionFromManager(s32, s32);
s32 SlotController_IsSlotIdle(s32);
s32 SunOrb_GetManagerStatus(void);
void Ratatosk_GetPlayerOffsetFarthestFromSelf(void *, void *);
s32 Ratatosk_IsPositionOutsideArena(void *);
void SunOrb_FadeOutAll(void *);
s32 Ratatosk_FindPointNearestPlayer(void *, s32);
void SunOrb_SetMotionParams(void *, s32, s32, s32, s32);
// confidence=low: raises a sun orb at a player-near point; precise attack name tentative.
void Ratatosk_UpdateSunOrbPlacement(struct Ratatosk *p)
{
    struct OrbConfig config;
    struct Ratatosk *q = p;
    if (Ratatosk_ConsumeActionEntry(q) == 1) {
        if (SunOrb_GetManagerStatus() == 0) {
            Ratatosk_GetPlayerOffsetFarthestFromSelf(p, &p->destination);
            Ratatosk_SetAnimation(q, 4);
            p->sequence = 0;
            p->repeat = 0;
            p->waitTimer = 0;
            p->timer = 0;
            p->orbTimer = 0;
            p->hasOrb = 0;
            p->orbReady = 0;
            gRandomIndex = (gRandomIndex + 1) & 1023;
            {
                s32 sample = (u8)((const u16 *)RANDOM_TABLE_ADDR)[gRandomIndex];
                p->orbParameter = sample;
            }
            if (p->orbParameter > 128)
                p->orbParameter = 1;
            else
                p->orbParameter = 0;
        } else {
            Ratatosk_SetAnimation(q, 12);
            p->sequence = 2;
            p->orbReady = 1;
        }
    }
    {
        s32 *sequence = &p->sequence;
        switch (*sequence) {
            case 0:
                if (q->actionFinished) {
                    if (Ratatosk_IsPositionOutsideArena(&q->body.position)) {
                        Ratatosk_EnterPhase(q, 11);
                    } else {
                        Ratatosk_SetAnimation(q, 11);
                        *sequence = 1;
                    }
                }
                break;
            case 1:
                if (q->actionFinished) {
                    Ratatosk_SetAnimation(q, 12);
                    *sequence = 2;
                }
                break;
            case 2:
                if (p->orbTimer > 5 && p->hasOrb == 0 && p->orbReady == 1)
                    q->actionFinished = 1;
                if (p->hasOrb == 1 && p->orbReady == 1) {
                    SunOrb_FadeOutAll(p->sunOrbs);
                    p->sequenceDone = 1;
                }
                if (q->actionFinished) {
                    if (p->hasOrb == 0) {
                        s32 point = Ratatosk_FindPointNearestPlayer(p, 0);
                        if (point < 0) {
                            p->sequenceDone = 1;
                            break;
                        }
                        RisingActorConfig_Initialize(&config);
                        ((u8 *)&config)[7] = point;
                        SunOrb_SetTargetSource(&config, 2);
                        SunOrb_SetMotionParams(&config, 360, 4, 0, 2);
                        ((u16 *)&config)[11] = p->orbParameter;
                        {
                            s32 slot =
                                SunOrb_CreateAtPosition(&config, &p->orbPoints[point].position);
                            if (slot >= 0) {
                                p->orbPoints[point].active = 1;
                                p->targets[slot].active = 1;
                                RisingActorManager_BeginRaise(slot);
                                p->selectedTarget = slot;
                            }
                        }
                        p->hasOrb = 1;
                    }
                    p->sequence = 4;
                } else
                    p->timer++;
                break;
            case 4:
                if ((s16)++p->timer > 600) {
                    SunOrb_FadeOutAll(p->sunOrbs);
                    p->sequenceDone = 1;
                }
                break;
        }
    }
}

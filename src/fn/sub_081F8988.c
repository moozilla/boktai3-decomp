#include "global.h"
#ifdef NONMATCHING
// fuzzy: 97.7%
struct RatatoskPosition {
    u32 xy, z;
};
struct OrbSlot {
    s32 active;
    u8 pad4[20];
};
struct Ratatosk {
    u8 pad0[0x54];
    struct RatatoskPosition position;
    u8 pad5C[0xaa - 0x5c];
    u8 animation, padAB[3], state, ready, active;
    u8 padB1[0x180 - 0xb1];
    s32 sequence, complete;
    u8 pad188[0xdf0 - 0x188];
    struct OrbSlot slots[3];
    u8 padE38[0xe4c - 0xe38];
    u16 selectedOrb;
    u8 padE4E[0xe5a - 0xe4e];
    u16 repeat;
    struct RatatoskPosition destination;
    u8 padE64[8];
    u16 duration;
    u8 padE6E[0xe84 - 0xe6e];
    u16 timer;
    u8 padE86[2];
    s16 hasOrb;
    u8 padE8A[8];
    s16 previousOrb;
    u8 padE94[0xede - 0xe94];
    s16 waitTimer;
    u8 padEE0[0xf2c - 0xee0];
    s32 orbManager;
};
struct OrbConfig {
    u32 words[8];
};
#define HALF(p, o)     (*(s16 *)((u8 *)(p) + (o)))
#define WORD(p, o)     (*(s32 *)((u8 *)(p) + (o)))
#define POSITION(p, o) (*(struct RatatoskPosition *)((u8 *)(p) + (o)))
s32 Ratatosk_GetSelectedPointIndex(void);
s32 SunOrb_IsFadingOut(void);
struct RatatoskPosition *SunOrb_GetPosition(s32);
void SlotController_SetSlotFlag2(s32);
s32 SunOrb_GetSlotState(s32);
void RisingActorConfig_Initialize(void *);
void SunOrb_SetTargetSource(void *, s32);
void Ratatosk_GetPlayerOffsetFarthestFromSelf(void *, void *);
void Ratatosk_GetPlayerOffsetNearestSelf(void *, void *);
s32 SunOrb_CreateAtPosition(void *, void *);
void RisingActorManager_BeginRaise(s32);
void sub_081FEC38(s32, s32);
struct RatatoskPosition *SunOrb_GetPositionFromManager(s32, s32);
s32 SlotController_IsSlotIdle(s32);
static inline u8 ConsumeActivation(struct Ratatosk *p)
{
    if (p->active) {
        p->active = 0;
        p->ready = 0;
        return 1;
    }
    return 0;
}
// confidence=low: coordinates orb selection, raising and relocation; exact attack role tentative.
void Ratatosk_UpdateSunOrbRelocation(struct Ratatosk *p)
{
    struct OrbConfig config;
    struct RatatoskPosition farPosition, nearPosition;
    struct Ratatosk *q = p;
    if (ConsumeActivation(q) == 1) {
        if (p->hasOrb == 1) {
            u32 animation = 10;
            q->animation = animation;
            p->sequence = 0;
            p->repeat = 0;
            p->waitTimer = 0;
        } else {
            s32 slot = Ratatosk_GetSelectedPointIndex();
            if (slot >= 0) {
                if (!SunOrb_IsFadingOut()) {
                    p->destination = *SunOrb_GetPosition(slot);
                    p->duration = 32;
                    {
                        u32 animation = 4;
                        q->animation = animation;
                    }
                    p->sequence = 1;
                } else {
                    p->complete = 1;
                    return;
                }
            } else {
                p->complete = 1;
                return;
            }
        }
    }
    switch (p->sequence) {
        case 0:
            if (q->ready) {
                s32 slot = Ratatosk_GetSelectedPointIndex();
                if (slot < 0) {
                    p->complete = 1;
                    return;
                }
                if (!SunOrb_IsFadingOut()) {
                    p->destination = *SunOrb_GetPosition(slot);
                    p->duration = 32;
                    {
                        u32 animation = 4;
                        q->animation = animation;
                    }
                    p->sequence = 1;
                } else {
                    p->complete = 1;
                    return;
                }
            }
            break;
        case 1:
            SlotController_SetSlotFlag2(Ratatosk_GetSelectedPointIndex());
            if (q->ready) {
                p->sequence = 2;
                p->waitTimer = 0;
            }
            break;
        case 2: {
            s32 slot = Ratatosk_GetSelectedPointIndex();
            if (SunOrb_GetSlotState(slot) > 60) {
                u32 animation = 13;
                q->animation = animation;
                p->sequence = 3;
                p->previousOrb = slot;
                p->selectedOrb = 0xffff;
                RisingActorConfig_Initialize(&config);
                SunOrb_SetTargetSource(&config, 1);
                Ratatosk_GetPlayerOffsetFarthestFromSelf(p, &farPosition);
                slot = SunOrb_CreateAtPosition(&config, &farPosition);
                if (slot >= 0) {
                    p->slots[slot].active = 1;
                    RisingActorManager_BeginRaise(slot);
                    p->selectedOrb = slot;
                }
            } else if (p->waitTimer > 60 || !SlotController_IsSlotIdle(slot)) {
                p->complete = 1;
                return;
            }
            p->waitTimer++;
        } break;
        case 3:
            SlotController_SetSlotFlag2(Ratatosk_GetSelectedPointIndex());
            if (q->ready) {
                sub_081FEC38(p->orbManager, p->previousOrb);
                p->position = *SunOrb_GetPositionFromManager(p->orbManager, HALF(p, 0xe4c));
                {
                    u32 animation = 14;
                    q->animation = animation;
                }
                p->sequence = 4;
            }
            break;
        case 4:
            SlotController_SetSlotFlag2(Ratatosk_GetSelectedPointIndex());
            if (q->ready) {
                Ratatosk_GetPlayerOffsetNearestSelf(p, &nearPosition);
                p->destination = nearPosition;
                {
                    u32 animation = 4;
                    q->animation = animation;
                }
                p->sequence = 5;
            }
            break;
        case 5:
            if (q->ready)
                p->complete = 1;
            break;
    }
    p->timer++;
}

#else
INCLUDE_ASM("asm/nonmatching", Ratatosk_UpdateSunOrbRelocation);
#endif

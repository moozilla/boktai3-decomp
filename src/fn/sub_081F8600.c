#include "global.h"
#include "engine.h"
#include "ram.h"

struct RatatoskPosition {
    u32 xy, z;
};
struct PointSlot {
    s32 active;
    struct RatatoskPosition position;
    u8 padC[12];
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
    u8 pad188[0xd18 - 0x188];
    struct PointSlot points[9];
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
s32 SunOrb_GetManagerStatus(void);
void Ratatosk_GetPlayerOffsetFarthestFromSelf(void *, void *);
s32 Ratatosk_IsPositionOutsideArena(void *);
void SunOrb_FadeOutAll(s32);
s32 Ratatosk_FindPointNearestPlayer(void *, s32);
void SunOrb_SetMotionParams(void *, s32, s32, s32, s32);
// confidence=low: raises a sun orb at a player-near point; precise attack name tentative.
void Ratatosk_UpdateSunOrbPlacement(struct Ratatosk *p)
{
    struct OrbConfig config;
    struct Ratatosk *q = p;
    if (ConsumeActivation(q) == 1) {
        if (SunOrb_GetManagerStatus() == 0) {
            Ratatosk_GetPlayerOffsetFarthestFromSelf(p, &p->destination);
            {
                u32 animation = 4;
                q->animation = animation;
            }
            p->sequence = 0;
            p->repeat = 0;
            p->waitTimer = 0;
            p->timer = 0;
            HALF(p, 0xe86) = 0;
            p->hasOrb = 0;
            HALF(p, 0xee2) = 0;
            gRandomIndex = (gRandomIndex + 1) & 1023;
            {
                s32 sample = (u8)((const u16 *)RANDOM_TABLE_ADDR)[gRandomIndex];
                HALF(p, 0xf0e) = sample;
            }
            if (HALF(p, 0xf0e) > 128)
                HALF(p, 0xf0e) = 1;
            else
                HALF(p, 0xf0e) = 0;
        } else {
            u32 animation = 12;
            q->animation = animation;
            p->sequence = 2;
            HALF(p, 0xee2) = 1;
        }
    }
    {
        s32 *sequence = &p->sequence;
        switch (*sequence) {
            case 0:
                if (q->ready) {
                    if (Ratatosk_IsPositionOutsideArena(&q->position)) {
                        {
                            u32 state = 11;
                            q->state = state;
                        }
                        WORD(q, 0xc4) = 0;
                        q->active = 1;
                    } else {
                        u32 animation = 11;
                        q->animation = animation;
                        *sequence = 1;
                    }
                }
                break;
            case 1:
                if (q->ready) {
                    u32 animation = 12;
                    q->animation = animation;
                    *sequence = 2;
                }
                break;
            case 2:
                if (HALF(p, 0xe86) > 5 && p->hasOrb == 0 && HALF(p, 0xee2) == 1)
                    q->ready = 1;
                if (p->hasOrb == 1 && HALF(p, 0xee2) == 1) {
                    SunOrb_FadeOutAll(p->orbManager);
                    p->complete = 1;
                }
                if (q->ready) {
                    if (p->hasOrb == 0) {
                        s32 point = Ratatosk_FindPointNearestPlayer(p, 0);
                        if (point < 0) {
                            p->complete = 1;
                            break;
                        }
                        RisingActorConfig_Initialize(&config);
                        ((u8 *)&config)[7] = point;
                        SunOrb_SetTargetSource(&config, 2);
                        SunOrb_SetMotionParams(&config, 360, 4, 0, 2);
                        ((u16 *)&config)[11] = HALF(p, 0xf0e);
                        {
                            s32 slot = SunOrb_CreateAtPosition(&config, &p->points[point].position);
                            if (slot >= 0) {
                                p->points[point].active = 1;
                                p->slots[slot].active = 1;
                                RisingActorManager_BeginRaise(slot);
                                HALF(p, 0xe4e) = slot;
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
                    SunOrb_FadeOutAll(p->orbManager);
                    p->complete = 1;
                }
                break;
        }
    }
}

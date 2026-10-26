#include "global.h"
#include "engine.h"
// MATCHING_CONCESSION: register-pinning
struct Vec {
    s16 x, y, z, pad;
};
struct Actor {
    u8 pad0[0x54];
    struct Vec position;
    u8 pad5C[0x34];
    u32 counter;
    u8 pad94[0x16];
    u8 animation;
    u8 padAB[0x3];
    u8 requested;
    u8 input;
    u8 active;
    u8 padB1[0x13];
    u32 timer;
    u8 padC8[0x4];
    u8 direction;
    u8 padCD[0xb3];
    s32 sequence;
    u32 finished;
    u8 pad188[0xcc4];
    s16 newSlot;
    s16 slot;
    u8 padE50[0xa];
    s16 completion;
    struct Vec destination;
    struct Vec target;
    s16 duration;
    u8 padE6E[0x6];
    s16 pending;
    u8 padE76[0xe];
    s16 elapsed;
    s16 phaseTime;
    s16 orbCreated;
    s16 playerAngle;
    s16 released;
    u8 padE8E[0x4];
    s16 oldSlot;
    u8 padE94[0x4a];
    s16 repeat;
    s16 particles;
    s16 reuse;
    u8 padEE4[0x2];
    s16 releaseAngle;
    u8 padEE8[0x26];
    u16 orbParam;
    u8 padF10[0x1c];
    void *manager;
};
static inline u8 ConsumeActivation(struct Actor *p)
{
    if (p->active) {
        p->active = 0;
        p->input = 0;
        return 1;
    }
    return 0;
}
extern const u16 gUnk_0203B400[];
static inline u32 Random(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u16 *)(gRandomIndex * 2 + (u32)gUnk_0203B400);
}
static inline u32 RandomByte(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u8 *)(gRandomIndex * 2 + (u32)gUnk_0203B400);
}
void Ratatosk_GetPlayerOffsetFarthestFromSelf(void *, void *);
s32 SunOrb_GetManagerStatus(void);
s32 Ratatosk_IsPositionOutsideArena(void *);
s32 Ratatosk_FindPointNearestPlayer(void *, s32);
void RisingActorConfig_Initialize(void *);
void SunOrb_SetTargetSource(void *, s32);
void SunOrb_SetMotionParams(void *, s32, s32, s32, s32);
s32 SunOrb_CreateAtPosition(void *, void *);
void RisingActorManager_BeginRaise(s32);
void SunOrb_FadeOutAll(void *);
s32 Ratatosk_GetSelectedPointIndex(void);
s32 SunOrb_IsFadingOut(void);
struct Vec *SunOrb_GetPosition(s32);
void SlotController_SetSlotFlag2(s32);
s32 SunOrb_GetSlotState(s32);
s32 SlotController_IsSlotIdle(s32);
void sub_081FEC38(void *, s32);
struct Vec *SunOrb_GetPositionFromManager(void *, s32);
void Ratatosk_GetPlayerOffsetNearestSelf(void *, void *);
void Ratatosk_UpdateSunOrbRelocation(struct Actor *p)
{
    struct Actor *q = p;
    u8 config[32];
    struct Vec far, near;
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
                if (!SunOrb_IsFadingOut()) {
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
                if (slot < 0)
                    goto fail;
                if (!SunOrb_IsFadingOut()) {
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
            }
            break;
        case 1:
            SlotController_SetSlotFlag2(Ratatosk_GetSelectedPointIndex());
            if (q->input) {
                {
                    register u32 offset asm("r2") = 0x180;
                    asm("" : "+r"(offset));
                    {
                        register s32 *dest asm("r1") = (s32 *)((u8 *)p + offset);
                        *dest = 2;
                    }
                }
                p->repeat = 0;
            }
            break;
        case 2: {
            s32 slot = Ratatosk_GetSelectedPointIndex();
            if (SunOrb_GetSlotState(slot) > 60) {
                u32 animation = 13;
                q->animation = animation;
                {
                    u32 offset = 0x180;
                    {
                        s32 *dest = (s32 *)((u8 *)p + offset);
                        *dest = 3;
                    }
                }
                p->oldSlot = slot;
                p->newSlot = -1;
                RisingActorConfig_Initialize(config);
                SunOrb_SetTargetSource(config, 1);
                Ratatosk_GetPlayerOffsetFarthestFromSelf(p, &far);
                slot = SunOrb_CreateAtPosition(config, &far);
                if (slot >= 0) {
                    u32 offset = slot * 24;
                    u32 *flags = (u32 *)((u8 *)p + 0xdf0);
                    *(u32 *)((u8 *)flags + offset) = 1;
                    RisingActorManager_BeginRaise(slot);
                    p->newSlot = slot;
                }
            } else {
                if (p->repeat > 60 || !SlotController_IsSlotIdle(slot)) {
                fail:
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
                void **manager = &p->manager;
                sub_081FEC38(*manager, p->oldSlot);
                p->position = *SunOrb_GetPositionFromManager(*manager, p->newSlot);
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

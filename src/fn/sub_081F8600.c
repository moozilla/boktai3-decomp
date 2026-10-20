#include "global.h"
#ifdef NONMATCHING
// fuzzy: 83.3%
// Remaining differences: member-offset constant reuse, slot-flag addressing and register allocation.
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
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
static inline u32 Random(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return *(const u16 *)(gUnk_03005308 * 2 + (u32)gUnk_0203B400);
}
static inline u32 RandomByte(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return *(const u8 *)(gUnk_03005308 * 2 + (u32)gUnk_0203B400);
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
#define Ratatosk_UpdateSunOrbPlacement sub_081F8600
static inline void SetHalf(s16 *p, s32 n)
{
    *p = n;
}
void Ratatosk_UpdateSunOrbPlacement(struct Actor *p)
{
    struct Actor *q = p;
    u8 config[32];
    s32 activated = ConsumeActivation(p);
    if (activated == 1) {
        s32 status = SunOrb_GetManagerStatus();
        if (status == 0) {
            Ratatosk_GetPlayerOffsetFarthestFromSelf(p, &p->destination);
            {
                u32 animation = 4;
                q->animation = animation;
            }
            p->sequence = status;
            SetHalf(&p->completion, status);
            SetHalf(&p->repeat, status);
            SetHalf(&p->elapsed, status);
            SetHalf(&p->phaseTime, status);
            SetHalf(&p->orbCreated, status);
            SetHalf(&p->reuse, status);
            p->orbParam = RandomByte();
            if ((s32)p->orbParam > 128)
                p->orbParam = activated;
            else
                p->orbParam = status;
        } else {
            {
                u32 animation = 12;
                q->animation = animation;
            }
            p->sequence = 2;
            p->reuse = activated;
        }
    }
    {
        s32 *sequence = &p->sequence;
        switch (*sequence) {
            case 0:
                if (q->input) {
                    if (Ratatosk_IsPositionOutsideArena(&q->position)) {
                        {
                            u32 state = 11;
                            q->requested = state;
                        }
                        q->timer = 0;
                        q->active = 1;
                    } else {
                        {
                            u32 animation = 11;
                            q->animation = animation;
                        }
                        *sequence = 1;
                    }
                }
                break;
            case 1:
                if (q->input) {
                    {
                        u32 animation = 12;
                        q->animation = animation;
                    }
                    *sequence = 2;
                }
                break;
            case 2:
                if (p->phaseTime > 5 && p->orbCreated == 0 && p->reuse == 1)
                    q->input = 1;
                if (p->orbCreated == 1 && p->reuse == 1) {
                    SunOrb_FadeOutAll(p->manager);
                    p->finished = 1;
                }
                if (q->input) {
                    s16 *created = &p->orbCreated;
                    if (*created == 0) {
                        s32 point = Ratatosk_FindPointNearestPlayer(p, 0), slot;
                        if (point < 0) {
                            p->finished = 1;
                            break;
                        }
                        RisingActorConfig_Initialize(config);
                        config[7] = point;
                        SunOrb_SetTargetSource(config, 2);
                        SunOrb_SetMotionParams(config, 360, 4, 0, 2);
                        *(u16 *)(config + 0x16) = p->orbParam;
                        slot = SunOrb_CreateAtPosition(config, ((u8 *)p + point * 24) + 0xd1c);
                        if (slot >= 0) {
                            {
                                u32 *pointFlags = (u32 *)((u8 *)p + 0xd18);
                                pointFlags[point * 6] = 1;
                            }
                            {
                                u32 *slotFlags = (u32 *)((u8 *)p + 0xdf0);
                                slotFlags[slot * 6] = 1;
                            }
                            RisingActorManager_BeginRaise(slot);
                            p->slot = slot;
                        }
                        *created = 1;
                    }
                    p->sequence = 4;
                } else
                    ++p->elapsed;
                break;
            case 4:
                if (++p->elapsed > 600) {
                    SunOrb_FadeOutAll(p->manager);
                    p->finished = 1;
                }
                break;
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081F8600);
#endif

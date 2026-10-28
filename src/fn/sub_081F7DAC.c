// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"
#include "input.h"
#define gRandomTable gUnk_0203B400
struct RatatoskPosition {
    u32 packedXY, packedZ;
};
struct Ratatosk {
    u8 pad0[0x54];
    s16 x, y, z;
    u8 pad5A[0xaa - 0x5a];
    u8 animation, mode, padAC[2], state, inputMode, active;
    u8 padB1[0xc4 - 0xb1];
    u32 timer;
    u8 padC8[0x17c - 0xc8];
    u32 animationTimer;
    s32 sequence;
    u32 animationFlag;
    u8 pad188[0x49c - 0x188];
    s32 animationIndex;
    u8 pad4A0[0xcc8 - 0x4a0];
    s32 cloneTimer;
    struct RatatoskPosition positions[9];
    u8 padD14[0xe5a - 0xd14];
    u16 repeatCount;
    struct RatatoskPosition position;
    u8 padE64[8];
    u16 duration;
    s16 cloneReady;
    u8 padE70[0xe7c - 0xe70];
    u16 recovery;
    u8 padE7E[0xe96 - 0xe7e];
    s16 pendingCount;
    u8 padE98[0xef8 - 0xe98];
    u16 spawnClone;
};
extern const u16 gUnk_0203B400[];
s32 Mod(s32, s32);
u32 HomingProjectile_GetActiveCount(void);
static inline u8 ConsumeActivation(struct Ratatosk *p)
{
    if (p->active) {
        p->active = 0;
        p->inputMode = 0;
        return 1;
    }
    return 0;
}
static inline u32 NextRandom(const u16 *table)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u16 *)(gRandomIndex * 2 + (u32)table);
}
static inline s32 RandomModulo(s32 divisor)
{
    s32 bound = divisor;
    return Mod(NextRandom(gUnk_0203B400), bound);
}
void PlayerClone_BeginActiveReactions(void);
u32 PlayerClone_GetActiveCount(void);
void Player_SetFlag740(void);
void Player_ClearFlag740(void);
// confidence=low: repeated relocations followed by clone-pool activation; attack label tentative.
void Ratatosk_UpdateCloneAttack(struct Ratatosk *p)
{
    struct Ratatosk *q = p;
    if (ConsumeActivation(q) == 1) {
        if (p->pendingCount > 0) {
            u32 animation = 10;
            q->animation = animation;
            p->sequence = 0;
        } else {
            u32 animation = 6;
            q->animation = animation;
            p->sequence = 1;
        }
        p->sequence = 0;
        p->repeatCount = 0;
    }
    {
        s32 *sequence = &p->sequence;
        switch (*sequence) {
            case 0:
                if (q->inputMode) {
                    *sequence = 1;
                    {
                        u32 animation = 6;
                        q->animation = animation;
                    }
                }
                break;
            case 1:
                if (q->inputMode) {
                    *sequence = 2;
                    {
                        u32 animation = 4;
                        q->animation = animation;
                    }
                    p->cloneTimer = 0;
                }
                break;
            case 2:
                if (q->inputMode) {
                    if (++p->repeatCount > 5) {
                        *sequence = 3;
                        {
                            u32 animation = 8;
                            q->animation = animation;
                        }
                        p->duration = 128;
                        {
                            register s32 value asm("r4");
                            value = RandomModulo(2230);
                            asm("" : "+r"(value));
                            value += 1060;
                            p->x = value;
                            value = RandomModulo(1710);
                            asm("" : "+r"(value));
                            value += 1320;
                            p->z = value;
                        }
                    } else {
                        p->duration = 32;
                        gRandomIndex = (gRandomIndex + 1) & 1023;
                        p->position =
                            *(struct RatatoskPosition *)((Mod(*(const u16 *)(gRandomIndex * 2 +
                                                                             (u32)gUnk_0203B400),
                                                              9) *
                                                              8 +
                                                          (u32)p) +
                                                         0xccc);
                        {
                            u32 animation = 4;
                            q->animation = animation;
                        }
                    }
                }
                break;
            case 3:
                if (p->cloneReady > 0) {
                    p->spawnClone = 1;
                    PlayerClone_BeginActiveReactions();
                }
                if (PlayerClone_GetActiveCount() == 0) {
                    p->animationFlag = 1;
                    p->recovery = 60;
                    Player_SetFlag740();
                } else {
                    Player_ClearFlag740();
                }
                p->cloneTimer++;
                break;
        }
    }
}

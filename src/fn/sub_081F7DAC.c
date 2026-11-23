#include "global.h"
#include "ratatosk.h"
#include "engine.h"
#include "input.h"
#include "ram.h"
s32 Mod(s32, s32);
u32 HomingProjectile_GetActiveCount(void);
static inline u8 ConsumeActivation(struct Ratatosk *p)
{
    if (p->actionEnter) {
        p->actionEnter = 0;
        p->actionFinished = 0;
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
    return Mod(NextRandom(gRandomTable), bound);
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
        if (p->healthPhase > 0) {
            u32 animation = 10;
            q->actionAnimation = animation;
            p->sequence = 0;
        } else {
            u32 animation = 6;
            q->actionAnimation = animation;
            p->sequence = 1;
        }
        p->sequence = 0;
        p->repeat = 0;
    }
    {
        s32 *sequence = &p->sequence;
        switch (*sequence) {
            case 0:
                if (q->actionFinished) {
                    *sequence = 1;
                    {
                        u32 animation = 6;
                        q->actionAnimation = animation;
                    }
                }
                break;
            case 1:
                if (q->actionFinished) {
                    *sequence = 2;
                    {
                        u32 animation = 4;
                        q->actionAnimation = animation;
                    }
                    p->cloneTimer = 0;
                }
                break;
            case 2:
                if (q->actionFinished) {
                    if (++p->repeat > 5) {
                        *sequence = 3;
                        {
                            u32 animation = 8;
                            q->actionAnimation = animation;
                        }
                        p->duration = 128;
                        {
                            s32 x = 1060;
                            x += RandomModulo(2230);
                            p->body.position.x = x;
                        }
                        {
                            s32 z = 1320;
                            z += RandomModulo(1710);
                            p->body.position.z = z;
                        }
                    } else {
                        p->duration = 32;
                        gRandomIndex = (gRandomIndex + 1) & 1023;
                        p->destination =
                            *(struct ContactPosition *)((Mod(*(const u16 *)(gRandomIndex * 2 +
                                                                            (u32)gRandomTable),
                                                             9) *
                                                             8 +
                                                         (u32)p) +
                                                        0xccc);
                        {
                            u32 animation = 4;
                            q->actionAnimation = animation;
                        }
                    }
                }
                break;
            case 3:
                if (p->frameDamage > 0) {
                    p->pendingAttack = 1;
                    PlayerClone_BeginActiveReactions();
                }
                if (PlayerClone_GetActiveCount() == 0) {
                    p->sequenceDone = 1;
                    p->flashTimer = 60;
                    Player_SetFlag740();
                } else {
                    Player_ClearFlag740();
                }
                p->cloneTimer++;
                break;
        }
    }
}

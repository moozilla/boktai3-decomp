#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "engine/engine.h"
#include "engine/input.h"
#include "engine/ram.h"
s32 Mod(s32, s32);
u32 HomingProjectile_GetActiveCount(void);
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
void Player_SetScriptToggle(void);
void Player_ClearScriptToggle(void);
// confidence=low: repeated relocations followed by clone-pool activation; attack label tentative.
void Ratatosk_UpdateCloneAttack(struct Ratatosk *p)
{
    struct Ratatosk *q = p;
    if (Ratatosk_ConsumeActionEntry(q) == 1) {
        if (p->healthPhase > 0) {
            Ratatosk_SetAnimation(q, 10);
            p->sequence = 0;
        } else {
            Ratatosk_SetAnimation(q, 6);
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
                    Ratatosk_SetAnimation(q, 6);
                }
                break;
            case 1:
                if (q->actionFinished) {
                    *sequence = 2;
                    Ratatosk_SetAnimation(q, 4);
                    p->cloneTimer = 0;
                }
                break;
            case 2:
                if (q->actionFinished) {
                    if (++p->repeat > 5) {
                        *sequence = 3;
                        Ratatosk_SetAnimation(q, 8);
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
                        Ratatosk_SetAnimation(q, 4);
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
                    Player_SetScriptToggle();
                } else {
                    Player_ClearScriptToggle();
                }
                p->cloneTimer++;
                break;
        }
    }
}

#include "global.h"
#include "ratatosk.h"
#include "engine.h"
#include "input.h"
#include "ram.h"
// Tentative family name: actor animation/state handlers; specific actor identity unknown.
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
void AnimationActor_UpdateProjectileSequence(struct Ratatosk *p)
{
    struct Ratatosk *q = p;
    if (ConsumeActivation(q)) {
        u32 animation = 6;
        q->actionAnimation = animation;
        p->sequence = 0;
    }
    {
        s32 *sequence = &p->sequence;
        switch (*sequence) {
            case 0:
                if (q->actionFinished) {
                    if (p->healthPhase > 0) {
                        u32 animation = 10;
                        q->actionAnimation = animation;
                        *sequence = 1;
                    } else
                        goto next;
                }
                break;
            case 1:
                if (q->actionFinished) {
                next:
                    *sequence = 2;
                    {
                        u32 animation = 8;
                        q->actionAnimation = animation;
                    }
                }
                break;
            case 2:
                if (q->actionFinished) {
                    p->duration = 128;
                    gRandomIndex = (gRandomIndex + 1) & 1023;
                    p->destination =
                        *(struct ContactPosition
                              *)((Mod(*(const u16 *)(gRandomIndex * 2 + (u32)gRandomTable), 9) * 8 +
                                  (u32)p) +
                                 0xccc);
                }
                if (HomingProjectile_GetActiveCount() == 0)
                    p->sequenceDone = 1;
                break;
        }
    }
}

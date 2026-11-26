#include "engine/shared/global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "engine/shared/engine.h"
#include "engine/shared/input.h"
#include "engine/shared/ram.h"
// Tentative family name: actor animation/state handlers; specific actor identity unknown.
s32 Mod(s32, s32);
u32 HomingProjectile_GetActiveCount(void);
void AnimationActor_UpdateProjectileSequence(struct Ratatosk *p)
{
    struct Ratatosk *q = p;
    if (Ratatosk_ConsumeActionEntry(q)) {
        Ratatosk_SetAnimation(q, 6);
        p->sequence = 0;
    }
    {
        s32 *sequence = &p->sequence;
        switch (*sequence) {
            case 0:
                if (q->actionFinished) {
                    if (p->healthPhase > 0) {
                        Ratatosk_SetAnimation(q, 10);
                        *sequence = 1;
                    } else
                        goto next;
                }
                break;
            case 1:
                if (q->actionFinished) {
                next:
                    *sequence = 2;
                    Ratatosk_SetAnimation(q, 8);
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

#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "engine/engine.h"
#include "engine/input.h"
#include "engine/ram.h"
#include "link/wireless/random.h"
// Tentative family name: actor animation/state handlers; specific actor identity unknown.
s32 Mod(s32, s32);
u32 HomingProjectile_GetActiveCount(void);
void AnimationActor_UpdateProjectileSequence(struct Ratatosk *p)
{
    struct Ratatosk *q = p;
    if (Ratatosk_ConsumeActionEntry(p)) {
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
                        p->sequence = 1;
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
                    p->destination = *(Mod(RANDOM_NEXT(), 9) + p->points);
                }
                if (HomingProjectile_GetActiveCount() == 0)
                    p->sequenceDone = 1;
                break;
        }
    }
}

#include "global.h"
#include "ratatosk.h"

struct RatatoskPosition {
    u32 xy, z;
};
s32 SunOrb_GetManagerStatus(void);
void Ratatosk_GetPlayerOffsetFarthestFromSelf(struct Ratatosk *, struct RatatoskPosition *);
u32 Ratatosk_IsPositionOutsideArena(struct RatatoskPosition *);

void Ratatosk_UpdateArenaCheckedSequence(struct Ratatosk *p)
{
    struct Ratatosk *q = p;
    if (Ratatosk_ConsumeActionEntry(q)) {
        if (SunOrb_GetManagerStatus() == 0) {
            Ratatosk_GetPlayerOffsetFarthestFromSelf(p, &p->destination);
            Ratatosk_SetAnimation(q, 4);
            p->sequence = 0;
            p->repeat = 0;
            p->waitTimer = 0;
            p->timer = 0;
        } else {
            Ratatosk_SetAnimation(q, 12);
            p->sequence = 2;
        }
    }
    {
        s32 *sequence = &p->sequence;
        switch (*sequence) {
            case 0:
                if (q->actionFinished) {
                    if (Ratatosk_IsPositionOutsideArena(&q->body.position)) {
                        Ratatosk_EnterPhase(q, 11);
                    } else {
                        Ratatosk_SetAnimation(q, 11);
                        *sequence = 1;
                    }
                }
                break;
            case 1:
                if (q->actionFinished) {
                    Ratatosk_SetAnimation(q, 12);
                    *sequence = 2;
                }
                break;
            case 2:
                if (q->actionFinished)
                    p->sequenceDone = 1;
                p->repeat++;
                break;
        }
    }
    p->timer++;
}

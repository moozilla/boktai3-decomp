#include "global.h"
#include "ratatosk.h"
// Tentative family name: actor animation/state handlers; specific actor identity unknown.
struct AnimationPosition {
    u32 packedXY, packedZ;
};
void Ratatosk_GetPlayerOffsetFarthestFromSelf(struct Ratatosk *, struct AnimationPosition *);
void Sound_PlaySong(u32);
void AnimationActor_UpdateRepeatedAttack(struct Ratatosk *p)
{
    struct Ratatosk *q = p;
    if (Ratatosk_ConsumeActionEntry(p)) {
        Ratatosk_GetPlayerOffsetFarthestFromSelf(p, &p->destination);
        Ratatosk_SetAnimation(q, 4);
        p->sequence = 0;
        p->repeat = 0;
        p->waitTimer = 0;
    }
    switch (p->sequence) {
        case 0:
            if (q->actionFinished) {
                p->sequence = 1;
                Ratatosk_SetAnimation(q, 10);
                Sound_PlaySong(0x55d);
            }
            break;
        case 1:
            if (q->actionFinished) {
                if (++p->waitTimer > 2) {
                    p->sequence++;
                    q->actionAnimation = 1;
                } else
                    Ratatosk_SetAnimation(q, 10);
            }
            break;
        case 2:
            p->sequenceDone = 1;
            p->repeat++;
            break;
    }
}

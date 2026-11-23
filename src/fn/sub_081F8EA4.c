#include "global.h"
#include "ratatosk.h"
struct ContactPosition *Player_GetEffectivePosition(u32);
struct ContactPosition *Ratatosk_FindPointFarthestFromPosition(void *, void *);
struct ContactPosition *Ratatosk_GetRandomMovementPoint(void *);
// confidence=low: repeats a retreat animation before returning to a random movement point.
void Ratatosk_UpdateRepeatedRetreat(struct Ratatosk *p)
{
    struct Ratatosk *actor = p;
    if (Ratatosk_ConsumeActionEntry(actor)) {
        Ratatosk_SetAnimation(actor, 10);
        p->sequence = 0;
        p->waitTimer = 0;
    }
    switch ((u32)p->sequence) {
        case 0:
            if (actor->actionFinished) {
                if (++p->waitTimer > 1) {
                    {
                        struct ContactPosition candidate, best;
                        s32 maximum = 0;
                        s16 i;
                        for (i = -1; i <= 1; i += 2) {
                            s32 dx, dz, distance;
                            struct ContactPosition *v = &candidate;
                            struct ContactPosition *self = &p->anchor;
                            *v = *Player_GetEffectivePosition(0);
                            v->x = v->x + i * 512;
                            dx = (&candidate)->x - self->x;
                            dz = (&candidate)->z - self->z;
                            distance = dx * dx + dz * dz;
                            if (distance >= maximum) {
                                maximum = distance;
                                best = candidate;
                            }
                        }
                        for (i = -1; i <= 1; i += 2) {
                            s32 dx, dz, distance;
                            struct ContactPosition *v = &candidate;
                            struct ContactPosition *self = &p->anchor;
                            *v = *Player_GetEffectivePosition(0);
                            v->z = v->z + i * 512;
                            dx = (&candidate)->x - self->x;
                            dz = (&candidate)->z - self->z;
                            distance = dx * dx + dz * dz;
                            if (distance >= maximum) {
                                maximum = distance;
                                best = candidate;
                            }
                        }
                        p->destination = best;
                    }
                    p->duration = 16;
                    actor->actionAnimation = 4;
                    p->sequence = 1;
                } else
                    Ratatosk_SetAnimation(actor, 10);
            }
            break;
        case 1:
            if (actor->actionFinished) {
                p->interval = 20;
                Ratatosk_SetAnimation(actor, 1);
                p->sequence = 2;
            }
            break;
        case 2:
            if (actor->actionFinished) {
                Ratatosk_SetAnimation(actor, 7);
                p->sequence = 3;
            }
            break;
        case 3:
            if (actor->actionFinished) {
                p->interval = 120;
                Ratatosk_SetAnimation(actor, 1);
                p->sequence = 4;
            }
            break;
        case 4:
            if (actor->actionFinished) {
                struct ContactPosition *point = Ratatosk_GetRandomMovementPoint(p);
                if (point) {
                    p->destination = *point;
                    p->duration = 64;
                }
                Ratatosk_SetAnimation(actor, 4);
                p->sequence = 5;
            }
            break;
        case 5:
            if (actor->actionFinished)
                p->sequenceDone = 1;
            break;
    }
    actor->actionTimer++;
}

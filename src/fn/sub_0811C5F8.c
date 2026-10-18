#include "global.h"
#include "actor_stats.h"
void Character_SetLowResourceColorEffect(void *, void *);
void ActorStats_ApplyPendingIncrement(struct ActorStats *p)
{
    if (p->pending) {
        if (p->mode == 3) {
            p->value += p->increment;
            p->value = p->value > p->limit ? p->limit : p->value;
        } else {
            p->amount += p->increment;
            p->amount = p->amount > p->amountLimit ? p->amountLimit : p->amount;
            Character_SetLowResourceColorEffect(p->state44, p->state48);
        }
        p->pending = 0;
    }
}

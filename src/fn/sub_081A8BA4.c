#include "vanargand_pair_attack.h"

void sub_08159164(s32);

void ActorSoundPhase_EnterPhase21(struct VanargandPairAttack *p)
{
    struct VanargandPairAttack *q = p;
    if (Vanargand_ConsumeActionEntry(p)) {
        if (p->state != 21)
            Vanargand_EnterAction(p, 21);
        sub_08159164(q->attachment);
    }
    p->actionTimer++;
}

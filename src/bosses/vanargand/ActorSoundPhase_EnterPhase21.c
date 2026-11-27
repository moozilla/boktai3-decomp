#include "bosses/vanargand/vanargand_pair_attack.h"

void Player_AddExperience(s32);

void ActorSoundPhase_EnterPhase21(struct VanargandPairAttack *p)
{
    struct VanargandPairAttack *q = p;
    if (Vanargand_ConsumeActionEntry(p)) {
        if (p->state != 21)
            Vanargand_EnterAction(p, 21);
        Player_AddExperience(q->attachment);
    }
    p->actionTimer++;
}

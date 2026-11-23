#include "vanargand_pair_attack.h"

void ScenePanel_UpdatePairConditions(void *);
void ScenePanel_ApplyPairTransition(void *);

void ActorSoundPhase_UpdateStop(struct VanargandPairAttack *p)
{
    struct VanargandPairAttack *q = p;
    if (Vanargand_ConsumeActionEntry(p))
        Vanargand_EnterAction(p, 15);
    if (p->finished) {
        ScenePanel_UpdatePairConditions(q);
        ScenePanel_ApplyPairTransition(q);
    }
}

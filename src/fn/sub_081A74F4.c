#include "vanargand_pair_attack.h"

u32 Trigger_ConsumePending(void *, u32);

void Vanargand_UpdateMode2(struct VanargandPairAttack *boss)
{
    struct VanargandPairAttack *owner = boss;
    if (Vanargand_ConsumeActionEntry(boss) == 1)
        Vanargand_EnterAction(boss, 2);
    if (boss->finished) {
        if (ScenePair_CountPositiveCounters(owner->pair) <= 1)
            Vanargand_EnterActionPhase(boss, 3);
        else
            Trigger_ConsumePending(boss->trigger, 1);
    }
}

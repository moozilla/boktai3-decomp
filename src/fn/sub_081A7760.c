#include "vanargand_pair_attack.h"

u32 Trigger_ConsumePending(void *, u32);

void Vanargand_UpdateIdleTransition(struct VanargandPairAttack *panel)
{
    if (Vanargand_ConsumeActionEntry(panel)) {
        Vanargand_EnterAction(panel, 1);
        Trigger_ConsumePending(panel->trigger, 1);
    }
    panel->actionTimer++;
}

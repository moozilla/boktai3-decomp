#include "vanargand_pair_attack.h"

u32 Trigger_ConsumePending(void *, u32);

void Vanargand_BeginExitTransition(struct VanargandPairAttack *panel)
{
    if (Vanargand_ConsumeActionEntry(panel)) {
        Vanargand_EnterAction(panel, 29);
        Trigger_ConsumePending(panel->trigger, 1);
    }
}

#include "vanargand_pair_attack.h"

u32 Trigger_ConsumePending(void *, u32);

void ScenePanel_UpdateTimedExit(struct VanargandPairAttack *panel)
{
    if (Vanargand_ConsumeActionEntry(panel))
        Vanargand_EnterAction(panel, 3);
    if (panel->actionTimer > 199)
        Trigger_ConsumePending(panel->trigger, 1);
    panel->actionTimer++;
}

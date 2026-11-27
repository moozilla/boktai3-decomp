#include "bosses/vanargand/vanargand_pair_attack.h"

u32 Trigger_ConsumePending(void *, u32);

void Vanargand_UpdateMode27(struct VanargandPairAttack *p)
{
    if (Vanargand_ConsumeActionEntry(p) == 1)
        Vanargand_EnterAction(p, 0x1b);
    if (p->finished)
        Trigger_ConsumePending(p->trigger, 1);
}

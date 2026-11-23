#include "vanargand_pair_attack.h"

u32 Trigger_ConsumePending(void *, u32);

void Vanargand_UpdateMode26(struct VanargandPairAttack *p)
{
    if (Vanargand_ConsumeActionEntry(p) == 1)
        Vanargand_EnterAction(p, 0x1a);
    if (p->finished)
        Trigger_ConsumePending(p->trigger, 1);
}

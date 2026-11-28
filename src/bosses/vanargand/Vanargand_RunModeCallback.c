#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"

extern u32 gUnk_08612014[];
void Callback_InvokeActor(void *, u32);

void Vanargand_RunModeCallback(struct VanargandPairAttack *boss)
{
    boss->modeInterrupted = 0;
    Callback_InvokeActor(boss, gUnk_08612014[boss->actionMode]);
}

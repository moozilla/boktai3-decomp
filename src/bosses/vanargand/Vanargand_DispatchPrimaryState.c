#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"

extern const u32 gUnk_08611F48[];
void Callback_InvokeActor(void *, u32);

void Vanargand_DispatchPrimaryState(struct VanargandPairAttack *boss)
{
    Callback_InvokeActor(boss, gUnk_08611F48[boss->actionPhase]);
}

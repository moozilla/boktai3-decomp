#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"

extern const u32 gUnk_08611FE8[];
void Callback_InvokeActor(void *, u32);

void Vanargand_RunSecondaryPhaseCallback(struct VanargandPairAttack *boss)
{
    Callback_InvokeActor(boss, gUnk_08611FE8[boss->actionPhase]);
}

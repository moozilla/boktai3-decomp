#include "engine/shared/global.h"
#include "engine/shared/ram.h"
#include "bosses/vanargand/vanargand_pair_attack.h"

/* Registry routine 4D12: puts the Vanargand body back into action phase 14 and
   animation 16 and clears its damage part's accumulated damage (tentative). */
void Vanargand_ResetFromScript(void)
{
    struct VanargandPairAttack *p = gSceneComposite;
    p->part.accumulatedDamage = 0;
    Vanargand_StartAction(p, 0, 0, 14);
    Vanargand_EnterAction(p, 16);
}

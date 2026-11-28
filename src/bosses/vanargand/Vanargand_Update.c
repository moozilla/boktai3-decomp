#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"
#include "engine/video/render.h"

void Vanargand_UpdateAnimationAndTargets(struct VanargandPairAttack *p);
void Vanargand_RunModeCallback(struct VanargandPairAttack *p);
void Vanargand_ApplyRequestedMode(struct VanargandPairAttack *p);
void Vanargand_UpdateStateCallback(struct VanargandPairAttack *p);
void Vanargand_UpdatePresentation(struct VanargandPairAttack *p);

void Vanargand_Update(struct VanargandPairAttack *p)
{
    if ((gEnginePauseFlags & 7) == 0) {
        Vanargand_UpdateAnimationAndTargets(p);
        Vanargand_RunModeCallback(p);
        Vanargand_ApplyRequestedMode(p);
        Vanargand_UpdateStateCallback(p);
        Vanargand_UpdatePresentation(p);
        p->attackCounter = ((p->attackCounter + 1) & 0xFFFF) + 1;
    }
}

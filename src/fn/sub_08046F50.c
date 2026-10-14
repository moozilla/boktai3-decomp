#include "damage_number_display.h"
#define DamageNumberGroup_AdvancePhase sub_08046F50
void DamageNumberGroup_AdvancePhase(struct DamageNumberGroup *group)
{
    ++group->phase;
    group->phaseFrame = 0;
}

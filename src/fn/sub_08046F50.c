#include "damage_number_display.h"
void DamageNumberGroup_AdvancePhase(struct DamageNumberGroup *group)
{
    ++group->phase;
    group->phaseFrame = 0;
}

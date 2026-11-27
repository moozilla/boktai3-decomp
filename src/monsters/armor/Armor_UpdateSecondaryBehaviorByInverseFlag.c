#include "monsters/armor/scene_combat_actor.h"
void sub_081885BC(void *);
void sub_0818911C(void *);
void sub_08188B7C(void *);
void Armor_UpdateFacingDecision(void *);
void Armor_UpdateSecondaryBehaviorByInverseFlag(struct SceneCombatActor *actor)
{
    if ((actor->flags & 0x200) == 0)
        Armor_UpdateFacingDecision(actor);
    else
        sub_08188B7C(actor);
}

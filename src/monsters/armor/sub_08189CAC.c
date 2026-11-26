#include "monsters/armor/scene_combat_actor.h"
void sub_081885BC(void *);
void sub_0818911C(void *);
void sub_08188B7C(void *);
void Armor_UpdateFacingDecision(void *);
void Armor_UpdatePrimaryBehaviorByInverseFlag(struct SceneCombatActor *actor)
{
    if ((actor->flags & 0x200) == 0)
        sub_0818911C(actor);
    else
        sub_081885BC(actor);
}

#include "global.h"
#include "monsters/armor/scene_combat_actor.h"

extern void *const gUnk_0824F0CC[];

void Armor_DispatchTypeUpdate(struct SceneCombatActor *actor)
{
    if (!(actor->flags & 0x80))
        Callback_InvokeActor(actor, gUnk_0824F0CC[actor->kind]);
}

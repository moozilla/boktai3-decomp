#include "npc/sabata_rider/sabata_rider.h"

struct RegistryActor *SabataRider_FindAvailableEntry(RegistryActorManager *manager)
{
    s32 index;
    for (index = 0; index < REGISTRY_ACTOR_SLOT_COUNT; index++) {
        if (manager->actors[index].control.active != 1)
            return &manager->actors[index];
    }
    return 0;
}

struct RegistryActor *SabataRider_FindEntryByName(RegistryActorManager *manager, u32 name)
{
    s32 index;
    for (index = 0; index < REGISTRY_ACTOR_SLOT_COUNT; index++) {
        if (manager->actors[index].control.active != 0 &&
            (u32)manager->actors[index].control.owner == name)
            return &manager->actors[index];
    }
    return 0;
}

void SabataRider_RegisterMessageReceiver(struct RegistryActor *actor)
{
    if (RegistryActor_MessagePauseBits(REGISTRY_ACTOR_MESSAGES_PAUSED) == 0)
        SceneMessages_RegisterReceiver(actor->control.messages, (u32)actor->control.owner, 0xc);
}

void SabataRider_SetKeyframes(struct RegistryActor *actor, u32 frame)
{
    Anim_SetKeyframes(&actor->model.view.parts[2], (u8 *)&actor->model.resource, (u16)frame, 1, 0);
}

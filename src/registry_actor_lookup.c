#include "registry_actor.h"

struct RegistryActor *RegistryA8CB_FindAvailableEntry(RegistryActorManager *manager)
{
    s32 index;
    for (index = 0; index < 1; index++) {
        if (manager->actors[index].control.active != 1)
            return &manager->actors[index];
    }
    return 0;
}

struct RegistryActor *RegistryA8CB_FindEntryByName(RegistryActorManager *manager, u32 name)
{
    s32 index;
    for (index = 0; index < 1; index++) {
        if (manager->actors[index].control.active != 0 &&
            (u32)manager->actors[index].control.owner == name)
            return &manager->actors[index];
    }
    return 0;
}

void RegistryA8CB_RegisterMessageReceiver(struct RegistryActor *actor)
{
    u32 mask = 0x800;
    if (((gEngineFlags | gScenePauseFlags) & mask) == 0)
        SceneMessages_RegisterReceiver(actor->control.messages, (u32)actor->control.owner, 0xc);
}

void RegistryA8CB_SetKeyframes(struct RegistryActor *actor, u32 frame)
{
    Anim_SetKeyframes(&actor->model.view.parts[2], (u8 *)&actor->model.resource, (u16)frame, 1, 0);
}

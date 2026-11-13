#include "registry_actor.h"

void RegistryA8CB_CreateFromScript(void)
{
    RegistryActorManager *manager = gActorManager;
    struct RegistryActor *actor;
    void *animation;
    u16 active;
    if (!manager) {
        manager = RegistryA8CB_EnsureManager();
        if (!manager)
            return;
    }
    actor = RegistryA8CB_FindAvailableEntry(manager);
    if (!actor)
        return;
    Memory_Clear(actor->stats.relativeOrigin, 16);
    actor->control.boostEffect = 0xFF;
    ActorScript_ParseParams(actor);
    animation = &actor->model;
    ActorSprite_InitPartDrawings(actor, animation, 0x3D99);
    CharacterPreview_InitializePalette(&actor->stats.pendingDamage, animation, actor->stats.state48[3]);
    SceneSlot_RegisterObject(&actor->stats.position);
    ActorCollision_InitBounds(actor);
    {
        void (*update)(struct RegistryActor *) = ActorPhysics_UpdateFallMotion;
        u8 *resetKeys = &actor->resetKeys;
        u32 zero = 0;
        active = 1;
        *resetKeys = active;
        actor->control.update = update;
        actor->fallTimer = zero;
    }
    RegistryA8CB_RegisterMessageReceiver(actor);
    {
        struct RegistryActorMotion *motion = &actor->control.motion;
        motion->flags = active | motion->flags;
    }
    Modifier_InitDrainState(&actor->stats.drain, &actor->model.view.parts[0].x, 0, 0);
    actor->control.active = active;
}

void RegistryA8CB_ResetFromScript(void)
{
    RegistryActorManager *manager = gActorManager;
    struct RegistryActor *actor;
    if (manager &&
        (actor = RegistryA8CB_FindEntryByName(manager, Script_GetKeywordValue('n', 0))) != 0) {
        struct RegistryActor *entry = actor;
        struct ModifierDisplayObject *part = actor->model.view.parts;
        s32 index;
        for (index = 4; index >= 0; part++, index--)
            Anim_Remove(part);
        SpatialIndex_RemoveEntry(entry->stats.contactHeader);
        {
            u32 mask = 0x800;
            if (!((gEngineFlags | gScenePauseFlags) & mask))
                SceneMessages_UnregisterReceiver(actor->control.messages);
        }
    }
}

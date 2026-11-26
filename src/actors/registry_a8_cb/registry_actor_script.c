#include "actors/registry_a8_cb/registry_actor.h"

void RegistryA8CB_CreateFromScript(void)
{
    RegistryActorManager *manager = gActorManager;
    struct RegistryActor *actor;
    void *animation;
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
    CharacterPreview_InitializePalette(&actor->stats.pendingDamage, animation,
                                       actor->stats.state48[3]);
    SceneSlot_RegisterObject(&actor->stats.position);
    ActorCollision_InitBounds(actor);
    RegistryActor_StartMotion(actor, ActorPhysics_UpdateFallMotion);
    RegistryA8CB_RegisterMessageReceiver(actor);
    RegistryActor_SetMotionFlags(REGISTRY_ACTOR_MOTION_COMMANDS, &actor->control.motion);
    Modifier_InitDrainState(&actor->stats.drain, &actor->model.view.parts[0].x, 0, 0);
    actor->control.active = 1;
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
            if (!RegistryActor_MessagePauseBits(REGISTRY_ACTOR_MESSAGES_PAUSED))
                SceneMessages_UnregisterReceiver(actor->control.messages);
        }
    }
}

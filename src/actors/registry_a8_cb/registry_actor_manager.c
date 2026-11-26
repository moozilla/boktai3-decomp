#include "actors/registry_a8_cb/registry_actor.h"

struct RegistryActor *RegistryA8CB_FindOwnedEntry(void *owner)
{
    RegistryActorManager *manager;
    s32 index;
    if (gActorManager == NULL)
        return NULL;
    manager = gActorManager;
    for (index = 0; index < REGISTRY_ACTOR_SLOT_COUNT; index++) {
        if (manager->actors[index].control.active == 1 &&
            manager->actors[index].control.owner == owner)
            return &manager->actors[index];
    }
    return NULL;
}

void RegistryA8CB_StartFallFromScript(void)
{
    struct RegistryActor *object;
    s32 id = Script_GetKeywordValue('n', -1);
    if (id < 0)
        return;
    object = RegistryA8CB_FindOwnedEntry((void *)id);
    if (object) {
        RegistryActor_StartMotion(object, ActorMotion_UpdateFallFrame);
        RegistryActor_SetMotionFlags(REGISTRY_ACTOR_MOTION_COMMANDS, &object->control.motion);
    }
}

static inline void ClearDrawFlags(struct RegistryActorMotion *draw, s32 mask)
{
    draw->flags &= mask;
}

void RegistryA8CB_DisableCommandsFromScript(void)
{
    s32 name;
    struct RegistryActor *object;

    name = Script_GetKeywordValue('n', -1);
    if (name < 0)
        return;
    object = RegistryA8CB_FindOwnedEntry((void *)name);
    if (object == NULL)
        return;
    ClearDrawFlags(&object->control.motion, ~REGISTRY_ACTOR_MOTION_COMMANDS);
}

void RegistryA8CB_ResetUpdateState(struct RegistryActor *actor)
{
    actor->control.updateFlags = 0;
    SceneActor_DispatchCommands(actor);
}

void RegistryA8CB_UpdateAnimation(struct RegistryActor *actor)
{
    u32 flags;
    Character_UpdateTimedEffect(&actor->stats.pendingDamage);
    if (actor == 0 || ((flags = actor->control.updateFlags) & 0x14) == 0) {
        (actor->control.ticks)++;
    }
}

s32 ActorAction_RunActiveCallbacks(RegistryActorManager *manager)
{
    s32 i;
    for (i = 0; i < REGISTRY_ACTOR_SLOT_COUNT; i++) {
        struct RegistryActor *actor = &manager->actors[i];
        if (actor->control.active == 1) {
            RegistryA8CB_ResetUpdateState(actor);
            Callback_InvokeActor(actor, actor->control.update);
            RegistryA8CB_UpdateAnimation(actor);
        }
    }
    return 0;
}
static inline void DestroyActors(RegistryActorManager *manager, s32 i, u32 mask)
{
    do {
        if (manager->actors[i].control.active == 1) {
            struct ModifierDisplayObject *anim =
                (void *)((u8 *)manager->actors[0].model.view.parts +
                         i * sizeof(struct RegistryActor));
            s32 j = 4;
            struct RegistryActor *actor;
            do {
                Anim_Remove(anim);
                anim++;
            } while (--j >= 0);
            actor = &manager->actors[i];
            SpatialIndex_RemoveEntry(actor->stats.contactHeader);
            if (!((gEngineFlags | gScenePauseFlags) & mask))
                SceneMessages_UnregisterReceiver(actor->control.messages);
        }
    } while (++i <= 0);
}

static inline void ClearManager(u32 zero)
{
    gActorManager = (void *)zero;
}

u8 ActorManager_Destroy(RegistryActorManager *manager)
{
    DestroyActors(manager, 0, REGISTRY_ACTOR_MESSAGES_PAUSED);
    ClearManager(0);
    return 0;
}

s32 RegistryA8CB_InitManager(RegistryActorManager *manager)
{
    gActorManager = manager;
    return 0;
}

RegistryActorManager *RegistryA8CB_EnsureManager(void)
{
    RegistryActorManager *manager;
    if (gActorManager != 0)
        return gActorManager;
    manager = Task_Create(0x8, 0x920);
    if (manager != 0) {
        Task_SetCallbacks(manager, (void (*)(void *))ActorAction_RunActiveCallbacks,
                          (void (*)(void *))ActorManager_Destroy);
        if (RegistryA8CB_InitManager(manager) < 0) {
            Task_Kill(manager);
            return 0;
        }
    }
    return manager;
}

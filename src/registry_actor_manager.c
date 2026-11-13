#include "registry_actor.h"

struct RegistryActor *RegistryA8CB_FindOwnedEntry(void *owner)
{
    RegistryActorManager *manager;
    s32 index;
    if (gActorManager == NULL)
        return NULL;
    manager = gActorManager;
    for (index = 0; index < 1; index++) {
        if (manager->actors[index].control.active == 1 && manager->actors[index].control.owner == owner)
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
        void (*update)(struct RegistryActor *) = ActorMotion_UpdateFallFrame;
        u8 *resetKeys = &object->resetKeys;
        u32 zero = 0;
        u16 one = 1;
        *resetKeys = one;
        object->control.update = update;
        object->fallTimer = zero;
        {
            struct RegistryActorMotion *draw = &object->control.motion;
            draw->flags = one | draw->flags;
        }
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
    ClearDrawFlags(&object->control.motion, ~1);
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
    struct RegistryActorControl *control = &manager->actors[0].control;
    struct RegistryActor *actor = manager->actors;
    s32 remaining = 0;
    do {
        if (control->active == 1) {
            RegistryA8CB_ResetUpdateState(actor);
            Callback_InvokeActor(actor, control->update);
            RegistryA8CB_UpdateAnimation(actor);
        }
        /* The embedded control advances with its complete actor slot. */
        do {
            control = (void *)((u8 *)control + sizeof(struct RegistryActor));
        } while (0);
        actor++;
    } while (--remaining >= 0);
    return 0;
}

static inline void DestroyActors(RegistryActorManager *manager, s32 i, u32 mask)
{
    do {
        if (manager->actors[i].control.active == 1) {
            struct ModifierDisplayObject *anim = (void *)((u8 *)manager->actors[0].model.view.parts + i * sizeof(struct RegistryActor));
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
    s32 i = 0;
    DestroyActors(manager, i, 0x800);
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
        Task_SetCallbacks(manager, (void (*)(void *))ActorAction_RunActiveCallbacks, (void (*)(void *))ActorManager_Destroy);
        if (RegistryA8CB_InitManager(manager) < 0) {
            Task_Kill(manager);
            return 0;
        }
    }
    return manager;
}

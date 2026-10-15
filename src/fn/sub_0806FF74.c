// MATCHING_CONCESSION: register-pinning
#include "scene_actor_registry.h"
#define SceneActorRegistry_EnsureActorSubsystems sub_0806FF74
#include "scene_actor_control.h"
#include "actor_attachment_manager.h"
#include "scene_particle_burst.h"
struct SceneActorControlManager *SceneActorControl_EnsureSingleton(void);
struct ActorAttachmentManagerView *ActorAttachment_CreateManager(void);
struct SceneParticleBurstPool *SceneParticleBurst_EnsureSingleton(u32, u32);
struct Task *sub_08121B40(void);
struct Task *sub_08122610(void);
static inline u8 HasFlag(struct SceneActorRegistryManager *manager, u32 mask)
{
    if (manager->flags & mask)
        return 1;
    return 0;
}
void SceneActorRegistry_EnsureActorSubsystems(struct SceneActorRegistryManager *manager,
                                              struct SceneActorRegistryActor *actor)
{
    u32 category = actor->category;
    if (category == 2) {
        if (!HasFlag(manager, 8)) {
            struct Task *task = (void *)SceneActorControl_EnsureSingleton();
            manager->controlTask = task;
            if (!task)
                return;
            {
                u32 mask = 8;
                u32 flags;
                asm("" : "+r"(mask));
                flags = manager->flags;
                flags |= mask;
                manager->flags = flags;
            }
            manager->controlTaskId = task->id;
        }
        if (actor->paletteOffset == 1 && !HasFlag(manager, 0x10)) {
            struct Task *task = sub_08121B40();
            manager->variantTask = task;
            if (!task)
                return;
            {
                u32 mask = 0x10;
                u32 flags;
                asm("" : "+r"(mask));
                flags = manager->flags;
                flags |= mask;
                manager->flags = flags;
            }
            manager->variantTaskId = task->id;
        }
    } else if (!HasFlag(manager, 0x20)) {
        struct Task *task = (void *)ActorAttachment_CreateManager();
        manager->attachmentTask = task;
        if (!task)
            return;
        {
            u32 mask = 0x20;
            u32 flags;
            asm("" : "+r"(mask));
            flags = manager->flags;
            flags |= mask;
            manager->flags = flags;
        }
        manager->attachmentTaskId = task->id;
    }
    if (category == 3)
        SceneParticleBurst_EnsureSingleton(0, 0);
    if (category == 14 && !HasFlag(manager, 0x1000)) {
        struct Task *task = sub_08122610();
        manager->category14Task = task;
        if (!task)
            return;
        {
            u32 mask = 0x1000;
            u32 flags;
            asm("" : "+r"(mask));
            flags = manager->flags;
            flags |= mask;
            manager->flags = flags;
        }
        manager->category14TaskId = task->id;
    }
}

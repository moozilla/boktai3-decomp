#include "actor_pool.h"

struct ActorPoolManager *ActorPool_GetManager(void)
{
    return gActorPoolManager;
}

int ActorPool_LinkObject(struct ActorPoolManager *manager, struct PooledActor *actor)
{
    if (manager->head)
        manager->head->next = actor;
    actor->next = 0;
    actor->previous = manager->head;
    manager->head = actor;
    manager->serial++;
    actor->updateSlot = manager->serial & 3;
    return 0;
}

int ActorPool_UnlinkObject(struct ActorPoolManager *manager, struct PooledActor *actor)
{
    if (actor->next)
        actor->next->previous = actor->previous;
    else
        manager->head = actor->previous;
    if (actor->previous)
        actor->previous->next = actor->next;
    return 0;
}

void ActorPool_RefreshTargets(struct ActorPoolManager *manager)
{
    int i;
    struct SceneActorBody **out = manager->targets;
    struct Player **src = gPlayers;
    for (i = 0; i < 4; i++) {
        struct Player *player = *src;
        *out = player ? (struct SceneActorBody *)&player->positionalEffectGroup : 0;
        out++;
        src++;
    }
}

int ActorPool_UpdateCallback(struct ActorPoolManager *manager)
{
    struct PooledActor *actor = manager->head;
    if (actor) {
        u32 mask = 1;
        do {
            struct PooledActor *next = actor->previous;
            if (actor->flags & mask) {
                if (actor->destroy)
                    actor->destroy(actor);
                ActorPool_UnlinkObject(manager, actor);
                Heap_Free(actor);
            } else if (actor->update)
                actor->update(actor);
            actor = next;
        } while (actor);
    }
    manager->ticks++;
    manager->updateSlot = manager->ticks & 3;
    return 0;
}

int ActorPool_DestroyCallback(struct ActorPoolManager *manager)
{
    struct PooledActor *actor = manager->head;
    while (actor) {
        struct PooledActor *next = actor->previous;
        actor->destroy(actor);
        ActorPool_UnlinkObject(manager, actor);
        Heap_Free(actor);
        actor = next;
    }
    gActorPoolManager = 0;
    return 0;
}

int ActorPool_InitManager(struct ActorPoolManager *manager)
{
    int i;
    gActorPoolManager = manager;
    {
        int zero = 0;
        for (i = 3; i >= 0; i--)
            manager->targets[i] = (void *)zero;
    }
    manager->head = 0;
    ActorPool_RefreshTargets(manager);
    return 0;
}

struct ActorPoolManager *ActorPool_CreateManager(void)
{
    struct ActorPoolManager *manager;
    if (gActorPoolManager != 0)
        return (void *)gActorPoolManager;
    manager = Task_Create(8, sizeof(*manager));
    if (manager != 0) {
        Task_SetCallbacks(manager, (void (*)(void *))ActorPool_UpdateCallback,
                          (void (*)(void *))ActorPool_DestroyCallback);
        if (ActorPool_InitManager(manager) < 0) {
            Task_Kill(manager);
            return 0;
        }
    }
    return manager;
}

struct PooledActor *ActorPool_AllocObject(int size)
{
    struct ActorPoolManager *manager = ActorPool_GetManager();
    struct PooledActor *actor;
    if (!manager) {
        manager = ActorPool_CreateManager();
        if (!manager)
            return 0;
    }
    actor = Heap_Alloc(size);
    if (!actor)
        return 0;
    Memory_Clear(actor, size);
    ActorPool_LinkObject(manager, actor);
    return actor;
}

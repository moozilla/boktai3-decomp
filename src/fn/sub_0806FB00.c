#include "scene_actor_registry.h"
#include "scene_actor_record.h"
#include "runtime.h"
struct SceneActorRegistryManager *SceneActorRegistry_GetManager(void);
struct ChaseActor *SceneActorRegistry_FindById(u32);
u8 SceneActorRecord_CanRecordPair(u32, u32);
void SceneActorRegistry_MarkCategoryOne(u8);
void TerrainOverlay_QueueAddActor(u32 *);
void SceneActorRegistry_EnsureActorSubsystems(struct SceneActorRegistryManager *,
                                              struct ChaseActor *);
static inline u8 TrackPairs(void)
{
    u32 mask = 4;
    if (gGameState->flags868 & mask)
        return 1;
    return 0;
}
u8 SceneActorRegistry_LinkActor(struct ChaseActor *actor)
{
    struct SceneActorRegistryManager *manager = SceneActorRegistry_GetManager();
    struct SceneActorRegistryNode *head, *node;
    if (!manager)
        return 0;
    head = gSceneActorRegistryNode;
    if (TrackPairs() &&
        !SceneActorRecord_CanRecordPair(gScriptEntryProcedure, (u16)actor->base.body.id))
        return 0;
    if (manager->field32 > 19 || SceneActorRegistry_FindById(actor->base.body.id))
        return 0;
    node = Heap_Alloc(8);
    Memory_Clear(node, 8);
    node->actor = actor;
    node->next = head;
    gSceneActorRegistryNode = node;
    SceneActorRegistry_MarkCategoryOne(actor->base.category);
    {
        u32 mask = 0x02000000;
        if (!(actor->base.statusFlags & mask))
            TerrainOverlay_QueueAddActor(&actor->base.body.id);
    }
    SceneActorRegistry_EnsureActorSubsystems(manager, actor);
    manager->field32++;
    manager->field62++;
    return 1;
}

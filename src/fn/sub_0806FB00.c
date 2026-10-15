// MATCHING_CONCESSION: register-pinning
#include "scene_actor_registry.h"
#define SceneActorRegistry_LinkActor sub_0806FB00
#include "scene_actor_record.h"
extern u16 gUnk_030039F0;
struct SceneActorRegistryManager *SceneActorRegistry_GetManager(void);
struct SceneActorRegistryActor *SceneActorRegistry_FindById(u32);
u8 SceneActorRecord_CanRecordPair(u32, u32);
void sub_0806FDBC(u8);
void sub_08003C00(u32 *);
void sub_0806FF74(struct SceneActorRegistryManager *, struct SceneActorRegistryActor *);
static inline u8 TrackPairs(void)
{
    u32 mask = 4;
    if (gUnk_02000710->flags868 & mask)
        return 1;
    return 0;
}
u8 SceneActorRegistry_LinkActor(struct SceneActorRegistryActor *actor)
{
    struct SceneActorRegistryManager *manager = SceneActorRegistry_GetManager();
    struct SceneActorRegistryNode *head, *node;
    if (!manager)
        return 0;
    head = gUnk_020004AC;
    if (TrackPairs() && !SceneActorRecord_CanRecordPair(gUnk_030039F0, (u16)actor->id))
        return 0;
    if (manager->field32 > 19 || SceneActorRegistry_FindById(actor->id))
        return 0;
    node = Heap_Alloc(8);
    Memory_Clear(node, 8);
    node->actor = actor;
    node->next = head;
    gUnk_020004AC = node;
    sub_0806FDBC(actor->category);
    {
        u32 mask = 0x02000000;
        asm("" : "+r"(mask));
        if (!(actor->flags2D0 & mask))
            sub_08003C00(&actor->id);
    }
    sub_0806FF74(manager, actor);
    manager->field32++;
    manager->field62++;
    return 1;
}

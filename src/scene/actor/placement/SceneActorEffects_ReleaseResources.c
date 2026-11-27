#include "scene/actor/scene_actor_effects.h"
#include "scene/motion/scene_actor_chase_shared.h"
void SceneMessages_UnregisterReceiver(void *);
void Shadow_Destroy(void *);
void SceneActorEffects_DestroyGroup(void *);
void TerrainOverlay_QueueRemoveActor(void *);
void SceneLink_RemoveNode(void *);
void SceneBody_Unregister(void *);
void SceneActorRegistry_UnlinkActor(struct ChaseActor *);
static inline u8 TestValue(u32 value)
{
    if (value)
        return 1;
    return 0;
}
void SceneActorEffects_ReleaseResources(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    u8 *drawable = actor->base.drawable;
    u16 *flags;
    void *identity;
    if (actor->base.drawableKind == 0)
        Sprite_Remove(drawable);
    else
        Anim_Remove(drawable + 0x20);
    Heap_Free(drawable);
    {
        u32 mask = 0x100;
        u16 *pointer;
        u32 value;
        pointer = &actor->base.placementFlags;
        value = *pointer & mask;
        flags = pointer;
        if (TestValue(value))
            SceneMessages_UnregisterReceiver(actor->base.receiver);
    }
    {
        u8 *shadow;
        shadow = actor->base.shadow;
        if (*shadow)
            Shadow_Destroy(shadow);
    }
    if (*(u8 *)group->effectConfig)
        SceneActorEffects_DestroyGroup(group->effectConfig);
    {
        u32 mask = 0x02000000;
        u32 flags;
        flags = actor->base.statusFlags & mask;
        identity = &actor->base.body;
        if (!flags)
            TerrainOverlay_QueueRemoveActor(identity);
    }
    if (SceneActorChase_TestField(flags, 0x200))
        SceneLink_RemoveNode(&group->common.placement);
    SceneBody_Unregister(identity);
    Collider_Remove(&actor->base.colliderA);
    SceneActorRegistry_UnlinkActor((void *)actor);
}

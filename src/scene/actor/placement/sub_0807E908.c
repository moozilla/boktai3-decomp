#include "scene/actor/scene_actor_effects.h"
#include "scene/actor/scene_actor_chase_shared.h"
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
void SceneActorEffects_ReleaseResources(struct SceneActorEffectsView *actor)
{
    u8 *control = actor->control;
    u8 *drawable = actor->drawable;
    u16 *flags;
    void *identity;
    if (actor->drawableKind == 0)
        Sprite_Remove(drawable);
    else
        Anim_Remove(drawable + 0x20);
    Heap_Free(drawable);
    {
        u32 mask = 0x100;
        u16 *pointer;
        u32 value;
        pointer = &actor->flags2C0;
        value = *pointer & mask;
        flags = pointer;
        if (TestValue(value))
            SceneMessages_UnregisterReceiver((u8 *)actor + 0xC);
    }
    {
        u8 *shadow;
        shadow = actor->shadow;
        if (*shadow)
            Shadow_Destroy(shadow);
    }
    if (*(u8 *)(control + 0xA8))
        SceneActorEffects_DestroyGroup(control + 0xA8);
    {
        u32 mask = 0x02000000;
        u32 flags;
        flags = actor->flags2D0 & mask;
        identity = (u8 *)actor + 0x50;
        if (!flags)
            TerrainOverlay_QueueRemoveActor(identity);
    }
    if (SceneActorChase_TestField(flags, 0x200))
        SceneLink_RemoveNode(control + 0x30);
    SceneBody_Unregister(identity);
    Collider_Remove((u8 *)actor + 0x134);
    SceneActorRegistry_UnlinkActor((void *)actor);
}

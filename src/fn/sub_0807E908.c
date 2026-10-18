#include "scene_actor_effects.h"
void SceneMessages_UnregisterReceiver(void *);
void Shadow_Destroy(void *);
void SceneActorEffects_DestroyGroup(void *);
void TerrainOverlay_QueueRemoveActor(void *);
void sub_0815F738(void *);
void SceneBody_Unregister(void *);
void SceneActorRegistry_UnlinkActor(struct SceneActorRegistryActor *);
static inline u8 TestFlag(u16 *flags, u32 mask)
{
    if (*flags & mask)
        return 1;
    return 0;
}
static inline u8 TestValue(u32 value)
{
    if (value)
        return 1;
    return 0;
}
void SceneActorEffects_ReleaseResources(struct SceneActorEffectsView *actor)
{
    u8 *control = (u8 *)actor + 0x3D4;
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
        u32 offset = 0x320;
        u8 *shadow;
        shadow = (u8 *)actor + offset;
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
    if (TestFlag(flags, 0x200))
        sub_0815F738(control + 0x30);
    SceneBody_Unregister(identity);
    Collider_Remove((u8 *)actor + 0x134);
    SceneActorRegistry_UnlinkActor((void *)actor);
}

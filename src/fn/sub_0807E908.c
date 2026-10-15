// MATCHING_CONCESSION: register-pinning
#include "scene_actor_effects.h"
void sub_08020D08(void *);
void sub_08013684(void *);
void sub_0823233C(void *);
void sub_08003C10(void *);
void sub_0815F738(void *);
void sub_08225938(void *);
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
#define SceneActorEffects_ReleaseResources sub_0807E908
void SceneActorEffects_ReleaseResources(struct SceneActorEffectsView *actor)
{
    u8 *control = (u8 *)actor + 0x3D4;
    register u8 *drawable asm("r4") = actor->drawable;
    register u16 *flags asm("r6");
    void *identity;
    if (actor->drawableKind == 0)
        Sprite_Remove(drawable);
    else
        Anim_Remove(drawable + 0x20);
    Heap_Free(drawable);
    {
        u32 mask = 0x100;
        register u16 *pointer asm("r1");
        u32 value;
        pointer = &actor->flags2C0;
        value = *pointer & mask;
        flags = pointer;
        if (TestValue(value))
            sub_08020D08((u8 *)actor + 0xC);
    }
    {
        register u32 offset asm("r2") = 0x320;
        u8 *shadow;
        shadow = (u8 *)actor + offset;
        if (*shadow)
            sub_08013684(shadow);
    }
    if (*(u8 *)(control + 0xA8))
        sub_0823233C(control + 0xA8);
    {
        u32 mask = 0x02000000;
        u32 flags;
        flags = actor->flags2D0 & mask;
        identity = (u8 *)actor + 0x50;
        if (!flags)
            sub_08003C10(identity);
    }
    if (TestFlag(flags, 0x200))
        sub_0815F738(control + 0x30);
    sub_08225938(identity);
    Collider_Remove((u8 *)actor + 0x134);
    SceneActorRegistry_UnlinkActor((void *)actor);
}

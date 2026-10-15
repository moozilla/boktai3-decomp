// MATCHING_CONCESSION: register-pinning
#include "scene_actor_registry.h"
#define SceneActorRegistry_TestCategoryFlags sub_08070F2C
struct SceneActorRegistryActor *sub_08070EB0(void);
struct SceneActorRegistryActor *sub_08070E00(u32);
static inline u32 IsUnavailable(struct SceneActorRegistryActor *actor, u32 disabled, u32 excluded)
{
    {
        register u32 savedExcluded asm("r6") = excluded;
        register struct SceneActorRegistryActor *owner asm("r1") = actor;
        if (owner) {
            u32 offset = 0x2d4;
            u32 value;
            asm("" : "+r"(offset));
            value = *(u16 *)((u8 *)owner + offset);
            value &= disabled;
            if (value)
                return 1;
            offset -= 4;
            {
                u32 *field = (void *)((u8 *)owner + offset);
                value = *field;
            }
            value &= savedExcluded;
            if (value || owner->health <= 0)
                return 1;
        }
        return 0;
    }
}
u32 SceneActorRegistry_TestCategoryFlags(s32 inputCategory, u32 inputFlags)
{
    register s32 category asm("r5") = inputCategory;
    register u32 flags asm("r4") = inputFlags;
    register struct SceneActorRegistryActor *actor asm("r1") = sub_08070EB0();
    if (actor)
        goto initialize;
    goto failure;
success:
    return 1;
initialize: {
    u32 disabled = 0x40;
    register u32 excluded asm("r6") = 0x20000;
    do {
        if (actor->category == category) {
            u32 unavailable = IsUnavailable(actor, disabled, excluded);
            asm("" : "+r"(unavailable));
            if (!unavailable && (actor->flags2CC & flags))
                goto success;
        }
        actor = sub_08070E00(actor->id);
    } while (actor);
}
failure:
    return 0;
}

#define SceneActor_UpdateAlternatingCollider sub_080AB6F4
#include "global.h"
#include "scene_actor_reset.h"
void sub_0821FF58(void *, s32, s32, s32, s32, s32);
static inline u32 ActorFlag(struct SceneActorRegistryResetView *actor, u32 input, u32 offset)
{
    u32 mask = input;
    return offset == 0x2D4 ? *(u16 *)((u8 *)actor + offset) & mask
                           : *(u32 *)((u8 *)actor + offset) & mask;
}
static inline u8 IsUnavailable(struct SceneActorRegistryResetView *actor)
{
    if (actor &&
        (ActorFlag(actor, 0x40, 0x2D4) || ActorFlag(actor, 0x20000, 0x2D0) || actor->health <= 0))
        return 1;
    return 0;
}
static inline void ApplyColliderB(struct ContactCollider *collider, u32 a, u32 b, u32 *source)
{
    u32 word = *source;
    u32 half = *(u16 *)((u8 *)source - 10);
    sub_0821FF58(collider, a, b, 0, word, half);
}
static inline void ApplyColliderA(struct ContactCollider *collider, u32 a, u32 b, u32 *source)
{
    u32 word = *source;
    u32 half = *(u16 *)((u8 *)source - 8);
    sub_0821FF58(collider, a, b, 0, word, half);
}
void sub_080AB6F4(struct SceneActorRegistryResetView *s)
{
    u32 mask;
    u32 flags;
    struct ContactCollider *collider;
    if (IsUnavailable(s))
        return;
    {
        u32 filter = 0xA027;
        flags = s->flags2D4;
        if (flags & filter)
            return;
    }
    mask = 0x100;
    if (flags & mask) {
        u32 a, b, word, half;
        u8 *source;
        collider = &s->colliderB;
        collider->flags &= ~mask;
        ApplyColliderB(collider, s->fieldB6, s->fieldBE, &s->wordD0);
    } else {
        u32 a, b, word, half;
        u8 *source;
        collider = &s->colliderB;
        collider->flags |= mask;
        ApplyColliderA(collider, s->fieldB4, s->fieldBC, &s->fieldCC);
    }
}

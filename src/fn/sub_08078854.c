// MATCHING_CONCESSION: register-pinning
#include "scene_actor_behavior.h"
#define SceneActorBehavior_ApplyContactReaction sub_08078854
void sub_0821FF8C(struct SceneActorBehaviorContactView *, struct SceneActorBehaviorContactView *);
void sub_0812F520(struct SceneActorBehaviorView *, struct SceneActorBehaviorContactView *,
                  struct SceneActorBehaviorContactView *);
void sub_08159860(struct SceneActorBehaviorContactView *, void *, u32);
static inline u32 Unavailable(struct SceneActorBehaviorView *actor)
{
    u32 result;
    if (actor) {
        register u32 mask asm("r1") = 0x40;
        u32 offset = 0x2D4;
        u32 value;
        value = *(u16 *)((u32)actor + offset);
        if (value & mask)
            goto unavailable;
        mask = 0x20000;
        {
            u32 offset = 0x2D0;
            value = *(u32 *)((u32)actor + offset);
        }
        if (value & mask)
            goto unavailable;
        if (actor->health > 0)
            goto available;
    unavailable:
        result = 1;
        goto done;
    }
available:
    result = 0;
done:
    asm("" : "+r"(result));
    return result;
}
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline u32 ReadReactionFlags(u32 inputMask, struct SceneActorBehaviorContactView *source)
{
    u32 mask = inputMask;
    return source->reactionFlags & mask;
}
void SceneActorBehavior_ApplyContactReaction(struct SceneActorBehaviorContactView *source,
                                             struct SceneActorBehaviorContactView *destination,
                                             struct SceneActorBehaviorView *actor)
{
    struct SceneActorBehaviorContactView *primary;
    register u32 unavailable asm("r3") = Unavailable(actor);
    if (unavailable)
        return;
    primary = (void *)&actor->primaryCollider;
    if (*(u16 *)((u8 *)actor + 0x178)) {
        if (actor->mode2E6) {
            destination->remaining = unavailable;
            destination->mode = 0;
        }
        return;
    }
    {
        u32 mask = 4;
        if (primary->flags & mask)
            return;
    }
    {
        u32 mask = 1;
        if (!(source->flags & mask))
            return;
    }
    sub_0821FF8C(source, destination);
    destination->parameter44 = source->parameter44;
    {
        u32 mode = destination->mode;
        u32 offset;
        offset = 0x2E6;
        *(u16 *)((u32)actor + offset) = mode;
    }
    if ((u32)(actor->category - 18) <= 2 || actor->category == 23) {
        destination->power = source->power;
    } else if (ReadReactionFlags(0x40000, source)) {
        u8 *defense = &actor->defense9F;
        s32 power;
        power = source->power;
        power -= *defense;
        if (power < 0)
            power = 0;
        destination->power = power;
    }
    sub_0812F520(actor, source, destination);
    actor->payload2E0 = source->kind;
    {
        register u32 parameter asm("r1") = source->parameter34;
        register u32 reaction asm("r2") = source->reactionFlags;
        u32 offset = 0x2D8;
        *(u32 *)((u32)actor + offset) = parameter;
        {
            u32 offset = 0x2DC;
            *(u32 *)((u32)actor + offset) = reaction;
        }
    }
    {
        u32 mask = 0x800;
        if (Test(gUnk_02000710->flags868, mask) || (ReadReactionFlags(0x40, source))) {
            destination->remaining = 0;
            destination->power = 0;
            actor->result2E4 = 0;
            actor->mode2E6 = 1;
        }
    }
    actor->result2E4 = destination->remaining;
    sub_08159860(source, destination, actor->reactionA0);
    {
        u32 mask = 0x2000;
        u32 offset = 0x2CC;
        u32 *field;
        field = (void *)((u32)actor + offset);
        *field |= mask;
    }
}

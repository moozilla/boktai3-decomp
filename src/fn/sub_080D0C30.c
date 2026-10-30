#include "global.h"
#include "scene_actor_reset.h"
// MATCHING_CONCESSION: register-pinning
/* Records attack contacts and queues a timed reaction (tentative). */
#define Mummy_OnAttackCollision sub_080D0C30
static inline u8 Mummy_IsAttackBlocked(u8 *c)
{
    u32 mask;
    u32 *p;
    u32 off;
    if (!c)
        goto zero;
    mask = 0x40;
    off = (u32) & ((struct SceneActorRegistryResetView *)0)->flags2D4;
    p = (u32 *)(c + off);
    if ((*((u16 *)p)) & mask)
        goto one;
    mask = 0x20000;
    off -= sizeof(u32);
    p = (u32 *)(c + off);
    if ((*p) & mask)
        goto one;
    if ((((struct SceneActorRegistryResetView *)c)->health) > 0)
        goto zero;
one:
    return 1;

zero:
    return 0;
}

void *SceneActorContacts_FindObjectById(u16);
static inline u32 Mummy_TestOwnerFlag(u32 *p, u32 mask)
{
    return *p & mask;
}
struct MummyContext {
    u8 unknown0[0x6F2];
    u16 flags;
};
struct MummyContactOwner {
    u8 unknown0[0x20];
    u32 flags;
};
static inline u8 Mummy_LoadContactLatch(struct MummyContext *state, u16 **saved, u32 mask)
{
    u16 *p = &state->flags;
    u32 v = *p & mask;
    *saved = p;
    if (v)
        return 1;
    return 0;
}
static inline u32 Mummy_ResponseAddress(u32 k_, void *base)
{
    return (u32)base + k_;
}

void Mummy_OnAttackCollision(u8 *unused, struct ContactCollider *obj, u8 *input)
{
    struct SceneActorRegistryResetView *base = (void *)input;
    struct MummyContext *state = ((struct SceneActorRegistryResetView *)input)->context;
    u16 responseCost;
    register u16 contactFlags asm("r2");
    register u32 rawKind asm("r1");
    u32 kindIndex;
    if (Mummy_IsAttackBlocked(input))
        return;
    rawKind = obj->contactFlags;
    kindIndex = (u16)(rawKind - 1);
    contactFlags = rawKind;
    if (kindIndex <= 7) {
        u16 *flags;
        if ((!Mummy_LoadContactLatch(state, &flags, 0x800)) &&
            Mummy_TestOwnerFlag(&((struct MummyContactOwner *)obj->owner)->flags, 8))
            return;
        {
            u32 mask = ~0x800;
            *flags &= mask;
        }
        if ((!Mummy_IsAttackBlocked(input)) && ((obj->contactFlags) & 15)) {
            ((struct SceneActorRegistryResetView *)input)->field304 = *(u16 *)obj->unknown4;
            {
                u32 *field = &((struct SceneActorRegistryResetView *)input)->flags2D0;
                u32 mask = 0xFEFFFFFF;
                *field &= mask;
            }
        }
        base->tracking.target = SceneActorContacts_FindObjectById(*(u16 *)&base->field304);
        {
            u32 value = obj->responseFlags;
            u32 mask = 1;
            mask |= value;
            obj->responseFlags = mask;
        }
        if ((!(*((u16 *)Mummy_ResponseAddress(
                (u32) & ((struct SceneActorRegistryResetView *)0)->colliderA.responseValue,
                base)))) &&
            Mummy_TestOwnerFlag(&((struct MummyContactOwner *)obj->owner)->flags, 0x200)) {
            base->field2E0 = 1;
            base->field2DC = 4;
            base->field2D8 = 0;
            {
                struct ContactCollider *collider = &base->colliderA;
                collider->cost = 1;
            }
            responseCost = *(volatile u16 *)&base->colliderA.cost;
            base->reactionSoundVariant = responseCost;
            base->colliderA.responseParameter = 1;
            base->colliderA.responseValue = 30;
            base->field2E6 = 4;
        }
    } else if ((!Mummy_IsAttackBlocked(input)) && (contactFlags & 15)) {
        ((struct SceneActorRegistryResetView *)input)->field304 = *(u16 *)obj->unknown4;
        {
            u32 *field = &((struct SceneActorRegistryResetView *)input)->flags2D0;
            u32 mask = 0xFEFFFFFF;
            *field &= mask;
        }
    }
}

#include "global.h"
#include "scene_actor_reset.h"
void *SceneActorContacts_SelectForActor(void *, u32);
void SceneActorControl_OnIdle(void);
static inline u32 SceneActorReaction_ReadFlags(struct ChaseActor *a, u32 mask, u32 **out)
{
    u32 *f = &a->base.flags2CC;
    u32 bits = *f & mask;
    *out = f;
    return bits;
}
static inline void SceneActorReaction_ClearBits(u32 *p, u32 mask)
{
    *p &= mask;
}
static inline void SceneActorReaction_SetBits(u16 *p, u32 mask)
{
    *p |= mask;
}
static inline u8 SceneActorReaction_HasContact(struct SceneActorReactionMotion *motion, u32 mask)
{
    if (motion->flags & mask)
        return 1;
    return 0;
}
static inline void SceneActorReaction_SetControl(struct ChaseActor *a, void *p, u32 kind, u32 zero,
                                                 u8 byteZero)
{
    a->base.field2B2 = 1;
    a->base.field2B0 = byteZero;
    a->base.field2B5 = kind;
    a->base.target = p;
    a->base.field2C8 = zero;
    SceneActorReaction_ClearBits(&a->base.flags114, ~1);
    a->base.word104 = zero;
}
// confidence=low: begins a contact reaction after filtering actor and contact flags.
#define SceneActorReaction_BeginContactResponse sub_0812F3CC
u8 SceneActorReaction_BeginContactResponse(struct ChaseActor *a)
{
    u8 *group = (u8 *)&a->group.common;
    void *contact;
    u32 *flags;
    u32 zero;
    u32 result;
    u8 byteZero;
    s32 reactionMask;
    {
        u32 mask = 0x1010;
        if (a->base.flags2D4 & mask)
            goto fail;
    }
    if (!SceneActorReaction_ReadFlags(a, 0x1000000, &flags))
        goto fail;
    if (!SceneActorReaction_HasContact((struct SceneActorReactionMotion *)&a->base.tracking.target,
                                       1))
        goto fail;
    {
        u32 mask = 0x80;
        result = *flags & mask;
        if (result)
            goto fail;
    }
    if (*(u8 **)((u8 *)a + 0x45C)) {
        u32 attachmentFlags = (*(u8 **)((u8 *)a + 0x45C))[5];
        if (attachmentFlags & 1)
            goto fail;
        {
            u32 mask = 4;
            mask &= attachmentFlags;
            if (mask)
                goto fail;
        }
    }
    if (!a->base.tracking.limit)
        goto fail;
    result = (u32)SceneActorContacts_SelectForActor(a, 1);
    contact = (void *)result;
    if (contact)
        goto success;
fail:
    return 0;

success: {
    u8 *control = group + 0x4C;
    do {
        SceneActorReaction_ClearBits(flags, 0xFFFEBE3F);
    } while (0);
    zero = 0;
    *(u32 *)(control + 0x18) = zero;
    byteZero = 0;
    *(u16 *)(control + 0x14) = zero;
    *(u16 *)(control + 8) = zero;
}

    *(void **)(group + 0x88) = contact;
    SceneActorReaction_SetControl(a, SceneActorControl_OnIdle, 0x10, zero, byteZero);
    {
        u32 state = 3;
        u8 *field = &a->base.state2A8;
        *field = state;
    }
    a->base.phase2A9 = 1;
    a->base.behavior2AA = byteZero;
    a->base.counter2AC = zero;
    a->base.field2B1 = 1;
    SceneActorReaction_ClearBits(flags, ~0x10);
    reactionMask = 0x800;
    SceneActorReaction_SetBits(&a->base.flags2D6, reactionMask);
    return 1;
}

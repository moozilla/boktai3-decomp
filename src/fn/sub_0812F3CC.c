// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reset.h"
#define FIELD(p, n, t) (*(t *)((u8 *)(p) + (n)))
void *SceneActorContacts_SelectForActor(void *, u32);
void SceneActorControl_OnIdle(void);
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
/* Offset reuse, gate-result lifetime and control reset registers still differ. */
static inline u32 Bits(u32 *p, u32 mask)
{
    return *p & mask;
}
static inline u32 ActorBits(struct SceneActorRegistryResetView *a, u32 mask)
{
    return a->flags2CC & mask;
}
static inline void Clear(u32 *p, u32 mask)
{
    *p &= mask;
}
static inline void Set(u16 *p, u32 mask)
{
    *p |= mask;
}
static inline u8 HasContact(struct SceneActorReactionMotion *motion, u32 mask)
{
    if (motion->flags & mask)
        return 1;
    return 0;
}
static inline void Control(struct SceneActorRegistryResetView *a, void *p, u32 kind, u32 zero,
                           u8 byteZero)
{
    a->field2B2 = 1;
    a->field2B0 = byteZero;
    a->field2B5 = kind;
    a->target = p;
    a->field2C8 = zero;
    Clear(&a->flags114, ~1);
    a->word104 = zero;
}
// confidence=low: begins a contact reaction after filtering actor and contact flags.
#define SceneActorReaction_BeginContactResponse sub_0812F3CC
u8 SceneActorReaction_BeginContactResponse(struct SceneActorRegistryResetView *a)
{
    register u8 *group asm("r5") = (u8 *)(&a->group);
    void *contact;
    u32 *flags;
    u32 zero;
    u8 byteZero;
    s32 t1;
    asm("" : "+r"(group));
    {
        u32 mask = 0x1010;
        if (a->flags2D4 & mask)
            goto fail;
    }
    if (!ActorBits(a, 0x1000000))
        goto fail;
    flags = &a->flags2CC;
    if (!HasContact((void *)(((u8 *)a) + 0xDC), 1))
        goto fail;
    {
        u32 mask = 0x80;
        if ((u8)((*flags) & mask))
            goto fail;
    }
    if (*((u8 **)(((u8 *)a) + 0x45C))) {
        u32 v = *((u8 *)(((u8 *)(*((u8 **)(((u8 *)a) + 0x45C)))) + 5));
        if (v & 1)
            goto fail;
        if (v & 4)
            goto fail;
    }
    if (!a->distanceLimitF8)
        goto fail;
    contact = SceneActorContacts_SelectForActor(a, 1);
    if (contact)
        goto success;
fail:
    return 0;

success: {
    u8 *control = group + 0x4C;
    do {
        Clear(flags, 0xFFFEBE3F);
    } while (0);
    zero = 0;
    *((u32 *)(((u8 *)control) + 0x18)) = zero;
    byteZero = 0;
    *((u16 *)(((u8 *)control) + 0x14)) = zero;
    *((u16 *)(((u8 *)control) + 8)) = zero;
}

    *((void **)(((u8 *)group) + 0x88)) = contact;
    Control(a, SceneActorControl_OnIdle, 0x10, zero, byteZero);
    {
        u32 state = 3;
        u8 *field = &a->state2A8;
        *field = state;
    }
    a->phase2A9 = 1;
    a->behavior2AA = byteZero;
    a->counter2AC = zero;
    a->field2B1 = 1;
    Clear(flags, ~0x10);
    t1 = 0x800;
    Set(&a->flags2D6, t1);
    return 1;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0812F3CC);
#endif

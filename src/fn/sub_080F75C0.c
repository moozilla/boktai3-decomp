// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_behavior.h"
struct AxeCollider {
    u8 pad[6];
    u16 flags;
    u8 rest[0x4C];
};
struct AxeContext {
    u8 pad0[0xA90];
    struct AxeCollider attack[3];
    struct AxeCollider body[3];
    u8 padC8C[0xCAB - 0xC8C];
    u8 flags;
    u8 padCAC[0xEB0 - 0xCAC];
    u8 timer;
};
void SceneActorBehavior_ApplyContactReaction(struct SceneActorBehaviorContactView *,
                                             struct SceneActorBehaviorContactView *, void *);
void ActorPhase_UpdateReverseFlicker(void);
#define H(p, off) (*(u16 *)((u8 *)(p) + (off)))
static inline u8 Unavailable(struct SceneActorRegistryResetView *actor)
{
    register u32 mask asm("r1");
    u32 offset;
    if (actor) {
        mask = 0x40;
        offset = 0x2D4;
        asm("" : "+r"(mask), "+r"(offset));
        if (*(u16 *)((u8 *)actor + offset) & mask)
            return 1;
        mask = 0x20000;
        offset -= 4;
        {
            u32 value = *(u32 *)((u8 *)actor + offset);
            if (value & mask)
                return 1;
        }
        if (actor->health <= 0)
            return 1;
    }
    return 0;
}
static inline void SetControl(struct SceneActorRegistryResetView *a, void *cb, u32 k)
{
    a->field2B2 = 1;
    a->field2B0 = 0;
    a->field2B5 = k;
    a->target = cb;
    a->field2C8 = 0;
    {
        u32 *p = &a->flags114;
        u32 mask = ~1;
        *p &= mask;
    }
    a->word104 = 0;
}
// confidence=low: the axe's body contact; records the hit, and while the axe is armed with a
// timer above 2, disables all six colliders and starts the reverse flicker.
#define Axe_OnBodyContact sub_080F75C0
#ifdef NONMATCHING
// fuzzy: 76.3%
// fix.py best (sweep after the inlined-insn fix)
// The ROM keeps the collider loop counter and hoists the SetControl addresses differently.
inline static u8 Unavailable_080F75C0(struct SceneActorRegistryResetView *actor)
{
    register u32 mask asm("r1");
    u32 offset;
    if (actor) {
        mask = 0x40;
        offset = 0x2D4;
        asm("" : "+r"(mask), "+r"(offset));
        if ((*((u16 *)(((u8 *)actor) + offset))) & mask)
            return 1;
        mask = 0x20000;
        offset -= 4;
        {
            s16 value = *((u32 *)(((u8 *)actor) + offset));
            if (value & mask)
                return 1;
        }
        if (actor->health <= 0)
            return 1;
    }
    return 0;
}

void Axe_OnBodyContact(struct SceneActorBehaviorContactView *source,
                       struct SceneActorBehaviorContactView *destination,
                       struct SceneActorRegistryResetView *actor)
{
    struct AxeContext *context = *((struct AxeContext **)(((u8 *)actor) + 0x3D0));
    typeof(destination->mode) t1;
    if (Unavailable_080F75C0(actor))
        return;
    SceneActorBehavior_ApplyContactReaction(source, destination, actor);
    if ((*((u16 *)(((u8 *)actor) + 0x178))) == 0) {
        *((u16 *)(((u8 *)actor) + 0x172)) = destination->remaining;
        t1 = destination->mode;
        *((u16 *)(((u8 *)actor) + 0x2E6)) = t1;
        *((u16 *)(((u8 *)actor) + 0x178)) = destination->parameter44;
    }
    if ((context->flags & 2) && (context->timer > 2)) {
        u32 i;
        for (i = 0; i <= 2; i++) {
            struct AxeCollider *a = &context->attack[i];
            struct AxeCollider *b = &context->body[i];
            a->flags |= 4;
            b->flags |= 4;
        }

        SetControl(actor, (void *)ActorPhase_UpdateReverseFlicker, 0x2F);
        context->timer = 0;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_080F75C0);
#endif

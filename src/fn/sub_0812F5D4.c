#include "global.h"
#include "scene_actor_chase.h"
#include "scene_actor_attachment.h"

// MATCHING_CONCESSION: register-pinning

// Remaining differences: PRE stack slots, random-loop allocation, and inline reset order.
#ifdef NONMATCHING
// fuzzy: 81.3%
// fix.py best (sweep after the inlined-insn fix)
static inline void SetState(struct SceneActorRegistryResetView *actor, u8 s, u8 p, u8 b)
{
    actor->state2A8 = s;
    actor->phase2A9 = p;
    actor->behavior2AA = b;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *cb, u8 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = cb;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
// confidence=low: advances a route probe and requests the contact-facing turn.
u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
s32 NavMesh_ConsumeProbeEvent(void *);
u8 SceneActorBehavior_RequestDescriptorTurn(void *, void *, u32, u32, u32);
u8 SceneActorBehavior_AccelerateTowardDescriptor(void *, void *, u32, u32, u32);
u8 sub_0807AF94(void *);
static inline u32 FullFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D4 & mask;
}
static inline u8 TestField(u16 *field, u32 mask)
{
    if (*field & mask)
        return 1;
    return 0;
}
static inline void BeginContactTurn(void *obj, u32 saved)
{
    struct SceneActorRegistryResetView *actor = obj;
    u8 *group = (u8 *)&actor->group;
    if (!FullFlags(actor, 0x400) && !TestField((u16 *)((u8 *)actor + 0x414), 4)) {
        register s32 prior asm("r5") = actor->body.direction;
        s32 tries = 0;
        s32 choice;
        const u16 *table = gRandomTable;
        u32 *randomIndex = &gRandomIndex;
        u32 wrap;
        register u32 mask asm("r6");
        do {
            u32 index;
            wrap = 1023;
            mask = 3;
            index = (*randomIndex + 1) & wrap;
            *randomIndex = index;
            choice = (table[index] & mask) << 6;
            if (choice != prior)
                goto chosen;
        } while (++tries <= 3);
        choice = prior;
    chosen:
        group[0x204] = choice;
        Chase_SavePacket(group + 0x8C, actor);
        group[0x205] = saved;
        SetControl(actor, *(void **)(group + 0x21C), 0x1B);
        SetState(actor, 3, 1, 0);
        group[0x206] = 2;
    }
}

static inline void SetBehavior(struct SceneActorRegistryResetView *actor, u32 behavior, u32 counter)
{
    actor->behavior2AA = behavior;
    actor->counter2AC = counter;
    actor->field2B1 = 1;
}
inline static u32 Arg1(u32 k_, struct SceneActorRegistryResetView *actor, u32 updateOffset)
{
    return (*((u16 *)(((u8 *)actor) + updateOffset))) & k_;
}

inline static void SetBehavior_1(struct SceneActorRegistryResetView *actor)
{
    actor->behavior2AA = 13;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

void SceneActorReaction_UpdateContactRoute(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)(&actor->group);
    u32 result;
    s32 event;
    u16 *scale;
    register u32 updateOffset asm("r4");
    if (Chase_TakeArrived(actor)) {
    }
    updateOffset = 0x270;
    asm("" : "+r"(updateOffset));
    if ((u8)Callback_InvokeActor(actor, *((void **)(((u8 *)actor) + updateOffset))))
        return;
    if (sub_0812F3CC(actor))
        return;
    result = Callback_InvokeActor(actor, *((void **)(((u8 *)actor) + 0x260)));
    updateOffset += 0x64;
    if (Arg1(0xa02f, actor, updateOffset)) {
        u16 *field = &actor->field2C2;
        *field = 64;
        scale = field;
    } else {
        u16 *field = &actor->field2C2;
        u32 value = *field;
        scale = field;
        if (!value)
            *field = 64;
    }
    if (!actor->drawableKind)
        Anim_SetDurationScale(((u8 *)actor->drawable) + 0x48, *scale);
    else
        Anim_SetFrameDurationScale(((u8 *)actor->drawable) + 0x20, *scale);
    event = NavMesh_ConsumeProbeEvent(*((void **)(group + 0x1fc)));
    if (event) {
        if (SceneActorBehavior_RequestDescriptorTurn(actor, *((void **)(group + 0x1fc)), 32, 32,
                                                     result)) {
            if (!TestField((u16 *)(group + 0x40), 4)) {
                if (sub_0807AF94(actor))
                    return;
                if (TestField((u16 *)(group + 0x40), 1)) {
                    u32 saved = ((((u8 *)actor)[0x3cd] + 5) & 7) << 5;
                    BeginContactTurn(actor, saved);
                    return;
                }
            }
            SetBehavior_1(actor);
        }
    } else if (SceneActorBehavior_AccelerateTowardDescriptor(actor, *((void **)(group + 0x1fc)), 32,
                                                             32, result)) {
        SetBehavior(actor, 14, event);
        return;
    }
    actor->counter2AC++;
}

#else
INCLUDE_ASM("asm/nonmatching", SceneActorReaction_UpdateContactRoute);
#endif

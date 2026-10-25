// MATCHING_CONCESSION: register-pinning
#include "scene_actor_chase.h"

#ifdef NONMATCHING
// fuzzy: 97.2%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
void SceneActor_SelectDistantRoutePoint(void *, void *, void *, u32);
u8 Vorn_SelectUphillTurn(void *, void *);
static inline u32 MotionFlags(struct SceneActorRegistryResetView *a, u32 mask)
{
    return a->flags2D4 & mask;
}
static inline u32 StatusFlags(struct SceneActorRegistryResetView *a, u32 mask)
{
    return a->flags2D0 & mask;
}
static inline u8 ContextFlags(u8 *p, u32 mask, u16 **out)
{
    u16 *field = (void *)(p + 0x6D4);
    u32 value = *field & mask;
    *out = field;
    if (value)
        return 1;
    return 0;
}
static inline u8 Unavailable(struct SceneActorRegistryResetView *a)
{
    if (a && (MotionFlags(a, 64) || StatusFlags(a, 0x20000) || a->health <= 0))
        return 1;
    return 0;
}
// confidence=low: periodically probes uphill destinations and queues a delayed route action.
// Remaining differences: flag pointer lifetimes, mask scheduling, and reset offsets.
static inline void Chase_SetState_1(struct SceneActorRegistryResetView *actor, u32 s, u32 p)
{
    actor->state2A8 = s;
    actor->phase2A9 = p;
    actor->behavior2AA = 0;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline u8 Chase_Test_(s16 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}

inline static u32 Chase_Bits32_1(struct SceneActorRegistryResetView *actor_, u32 mask)
{
    u32 *field = &actor_->flags2CC;
    return (*field) & mask;
}

void Vorn_TryUphillRoute(struct SceneActorRegistryResetView *actor)
{
    u8 *context = actor->context;
    u8 *group = context + 0x3D4;
    u16 *flags;
    struct ContactPosition *point;
    if (Unavailable(actor))
        return;
    if (MotionFlags(actor, 0xE3BF))
        return;
    if (Chase_Bits32_1(actor, 16))
        return;
    if (actor->field2B5 != 1)
        return;
    if (ContextFlags(context, 0x1D, &flags))
        return;
    if (*((u16 *)(context + 0x6BC))) {
        --(*((u16 *)(context + 0x6BC)));
        return;
    }
    *((u16 *)(context + 0x6BC)) = *((u16 *)(context + 0x6BE));
    if (((s32)Chase_RandomBits(255)) <= 189)
        return;
    if (Chase_Test_(*flags, 0x1D))
        return;
    point = (void *)(context + 0x6B0);
    SceneActor_SelectDistantRoutePoint(context, point, &actor->position, 0x400000);
    if (!Vorn_SelectUphillTurn(context, point))
        return;
    if (!Chase_TryRoute(actor, *((void **)(context + 0x5D0)), &actor->position, point)) {
        Callback_InvokeActor(actor, *((void **)(context + 0x5F8)));
        return;
    }
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    Chase_SetState_1(actor, 3, 1);
    Chase_SetCtl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    {
        u32 duration = (((u16)Chase_RandomBits(0xFFFF)) & 15) + 30;
        struct SceneActorReactionCommand *command = (void *)(group + 0x8C);
        void *payload = actor->entryTarget228;
        u32 kind = 19;
        command->duration = duration;
        command->payload = payload;
        command->first = 0;
        command->second = 0;
        command->kind = kind;
    }
    {
        register u32 mask asm("r0") = 8;
        asm("" : "+r"(mask));
        *flags = mask | (*flags);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Vorn_TryUphillRoute);
#endif

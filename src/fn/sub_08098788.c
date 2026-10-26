// MATCHING_CONCESSION: register-pinning
#include "global.h"

#ifdef NONMATCHING
// fuzzy: 83.2%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Remaining differences: address scheduling, inlined tests and register allocation.
#include "scene_actor_reset.h"
#include "engine.h"
extern u8 *gUnk_02000580;
extern s32 gUnk_02000478;
extern u16 gUnk_02000534, gUnk_02000500;
void ActorShadow_Disable(void *);
void Ghost_TickVisibilityTransition(void *);
void ActorTick_DecrementCounterAA(void *);
void ActorVisibility_TickRevealDelay(void *);
void Ghost_UpdateRandomDestination(void *);
void Ghost_ReactToPlayerMovement(void *);
void SceneActor_EasePlayerHeight(void *);
static inline u8 Flag(u8 *p, u32 mask)
{
    if (*(u32 *)(p + 0x558) & mask)
        return 1;
    return 0;
}
static inline u32 ReadFlags16(struct SceneActorRegistryResetView *actor, u32 inputMask)
{
    u32 mask = inputMask;
    u32 offset = 0x2D4;
    u16 *flags;
    flags = (void *)((u8 *)actor + offset);
    return *flags & mask;
}
static inline u32 ReadFlags32(struct SceneActorRegistryResetView *actor, u32 inputMask)
{
    u32 mask = inputMask;
    u32 offset = 0x2D0;
    u32 *flags;
    flags = (void *)((u8 *)actor + offset);
    return *flags & mask;
}
static inline u8 Unavailable(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    if (actor) {
        if (ReadFlags16(actor, 0x40) || ReadFlags32(actor, 0x20000) || actor->health <= 0)
            return 1;
    }
    return 0;
}
static inline u32 Hidden(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    u32 mask = 1;
    register u32 result asm("r0");
    if (actor->drawableKind == 0) {
        u32 flags = *(u32 *)actor->drawable & mask;
        if (flags) {
            result = 1;
            goto done;
        }
        asm("" ::: "r1");
    } else {
        u32 flags = *(u32 *)((u8 *)actor->drawable + 0x28) & mask;
        if (flags) {
            result = 1;
            goto done;
        }
    }
    result = 0;
done:
    return result;
}
static inline u8 PlayerMode(void)
{
    u8 *p = gUnk_02000580;
    if (p) {
        u32 result = 0;
        if (*(u16 *)(p + 0x538) && *(u16 *)(p + 0x534) == 4)
            result = 1;
        return result;
    }
    return 0;
}
static inline u8 GlobalMode(void)
{
    u32 result = 0;
    if (gUnk_02000534 && gUnk_02000500 == 4)
        result = 1;
    return result;
}
static inline u32 PlayerFlag(u8 *p, u32 mask)
{
    return *(u32 *)(p + 0x448) & mask;
}
static inline u8 SaveFlag(u32 mask)
{
    if (*(u32 *)((u8 *)gGameState + 0x868) & mask)
        return 1;
    return 0;
}
// confidence=low: coordinates Ghost visibility transitions with player reveal state.
static inline u32 PlayerFlag_1(u8 *p)
{
    return (*((u32 *)(p + 0x448))) & 0x10;
}

u32 Ghost_UpdateVisibility(struct SceneActorRegistryResetView *a)
{
    struct SceneActorRegistryResetView *saved = a;
    u8 *ctx;
    u32 k1;
    {
        struct ContactCollider *c = &a->colliderB;
        c->flags |= 4;
    }
    if (ReadFlags16(a, 0x4000)) {
        if (Flag((u8 *)a, 0x200)) {
            *((u32 *)a->drawable) |= 1;
            ActorShadow_Disable(((u8 *)a) + 0x320);
            {
                s8 mask = ~0x20;
                u32 *flags = (void *)(((u8 *)saved) + (k1 = 0x558));
                *flags &= mask;
            }
        } else
            Ghost_TickVisibilityTransition(a);
        ActorTick_DecrementCounterAA(a);
        goto finish;
    }
    ctx = a->context;
    if (!Unavailable(a)) {
        u32 discarded = gEngineFlags & 0x4800;
        u32 *flags = (u32 *)(ctx + k1);
        s32 reveal;
        if (Flag(ctx, 0x100)) {
            reveal = SaveFlag(0x20);
        } else
            reveal = *((s16 *)(((u8 *)gGameState) + 0x876));
        asm("" : : "r"(discarded));
        if (reveal > 0) {
            u32 mode;
            if (gEngineFlags & 0x800)
                mode = PlayerMode();
            else {
                if (gUnk_02000478 > 0)
                    goto hide;
                if (gUnk_02000580 && PlayerFlag_1(gUnk_02000580))
                    goto hide;
                mode = GlobalMode();
            }
            if (mode)
                goto hide;
            if (!Flag(ctx, 0x40)) {
                u32 hidden = Hidden(a);
                if (!hidden) {
                    *((u16 *)(ctx + 0x5B2)) = hidden;
                    {
                        u32 mask = ~0x20;
                        u32 value = (*flags) & mask;
                        *flags = value | 0x40;
                    }
                }
            }
        } else {
        hide:
            if (!Flag(ctx, 0x20)) {
                if ((u8)Hidden(a)) {
                    *((u16 *)(ctx + 0x5B2)) = 0;
                    {
                        u32 mask = ~0x40;
                        u32 value = (*flags) & mask;
                        *flags = value | 0x20;
                    }
                }
            }
        }
    }
    Ghost_TickVisibilityTransition(a);
    ActorVisibility_TickRevealDelay(a);
    Ghost_UpdateRandomDestination(a);
    Ghost_ReactToPlayerMovement(a);
finish:
    SceneActor_EasePlayerHeight(a);

    return 1;
}

#else
INCLUDE_ASM("asm/nonmatching", Ghost_UpdateVisibility);
#endif

#include "global.h"

#include "scene_actor_reset.h"
#include "scene_actor_control_helpers.h"
#include "engine.h"
#include "player.h"
#include "ram.h"
void ActorShadow_Disable(void *);
void Ghost_TickVisibilityTransition(void *);
void ActorTick_DecrementCounterAA(void *);
void ActorVisibility_TickRevealDelay(void *);
void Ghost_UpdateRandomDestination(void *);
void Ghost_ReactToPlayerMovement(void *);
void SceneActor_EasePlayerHeight(void *);
struct GhostVisibilityContext {
    u8 unknown0[0x558];
    u32 flags;
    u8 unknown55C[0x56];
    u16 elapsed;
};

static inline u8 Ghost_TestVisibilityFlags(struct GhostVisibilityContext *p, u32 mask)
{
    if (p->flags & mask)
        return 1;
    return 0;
}
static inline u32 Ghost_ReadMotionFlags(struct SceneActorRegistryResetView *actor, u32 inputMask)
{
    u32 mask = inputMask;
    u16 *flags = &actor->flags2D4;
    return *flags & mask;
}
static inline u32 Ghost_ReadStatusFlags(struct SceneActorRegistryResetView *actor, u32 inputMask)
{
    u32 mask = inputMask;
    u32 *flags = &actor->flags2D0;
    return *flags & mask;
}
static inline u8 Ghost_IsUnavailable(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    if (actor) {
        if (Ghost_ReadMotionFlags(actor, 0x40) || Ghost_ReadStatusFlags(actor, 0x20000) ||
            actor->health <= 0)
            return 1;
    }
    return 0;
}
static inline u8 Ghost_IsHidden(struct SceneActorRegistryResetView *actor)
{
    u32 mask = 1;
    if (!actor->drawableKind) {
        if (*(u32 *)actor->drawable & mask)
            goto hidden;
        goto visible;
    }
    if (!(*(u32 *)((u8 *)actor->drawable + 0x28) & mask))
        goto visible;
hidden:
    return 1;
visible:
    return 0;
}
static inline u8 Ghost_IsLocalPlayerRevealing(void)
{
    struct Player *p = gPlayers[0];
    if (p) {
        u32 state = p->active;
        u32 result = 0;
        if (state && p->sceneMode == 4)
            result = 1;
        return result;
    }
    return 0;
}
static inline u8 Ghost_IsGlobalRevealActive(void)
{
    u32 state = gPlayerPassageState;
    u32 result = 0;
    if (state && gPlayerInteractionMode == 4)
        result = 1;
    return result;
}
static inline u8 Ghost_TestSaveFlags(u32 mask)
{
    if (gGameState->flags868 & mask)
        return 1;
    return 0;
}
static inline u32 Ghost_ReadPlayerRevealFlag(struct Player *p)
{
    u32 *flags = &p->actionFlags;
    u32 mask = 0x10;
    return *flags & mask;
}

static inline u32 Ghost_TestGlobalFlags(u32 flags, u32 mask)
{
    return flags & mask ? 1 : 0;
}

static inline void Ghost_ClearVisibilityFlags(struct GhostVisibilityContext *context, u32 bits)
{
    u32 mask = ~bits;
    context->flags &= mask;
}
static inline void Ghost_SetVisibilityFlags(struct GhostVisibilityContext *context, u32 bits)
{
    context->flags |= bits;
}

u32 Ghost_UpdateVisibility(struct SceneActorRegistryResetView *a)
{
    struct SceneActorRegistryResetView *saved = a;
    struct GhostVisibilityContext *ctx;
    {
        struct ContactCollider *c = &a->colliderB;
        c->flags |= 4;
    }
    if (Ghost_ReadMotionFlags(a, 0x4000)) {
        if (Ghost_TestVisibilityFlags((void *)a, 0x200)) {
            *((u32 *)a->drawable) |= 1;
            ActorShadow_Disable(a->shadow);
            {
                s8 mask = ~0x20;
                u32 *flags = &((struct GhostVisibilityContext *)saved)->flags;
                *flags &= mask;
            }
        } else
            Ghost_TickVisibilityTransition(a);
        ActorTick_DecrementCounterAA(a);
        goto finish;
    }
    ctx = a->context;
    if (!Ghost_IsUnavailable(a)) {
        s32 reveal;
        Ghost_TestGlobalFlags(gEngineFlags, 0x4800);
        if (Ghost_TestVisibilityFlags(ctx, 0x100)) {
            reveal = Ghost_TestSaveFlags(0x20);
        } else
            reveal = gGameState->unk876;
        if (reveal > 0) {
            u32 mode;
            if (gEngineFlags & 0x800)
                mode = Ghost_IsLocalPlayerRevealing();
            else {
                if (gWeatherCollisionTimer > 0)
                    goto hide;
                if (gPlayers[0] && Ghost_ReadPlayerRevealFlag(gPlayers[0]))
                    goto hide;
                mode = Ghost_IsGlobalRevealActive();
            }
            if (mode)
                goto hide;
            if (!Ghost_TestVisibilityFlags(ctx, 0x40)) {
                u32 hidden = Ghost_IsHidden(a);
                if (!hidden) {
                    ctx->elapsed = hidden;
                    Ghost_ClearVisibilityFlags(ctx, 0x20);
                    Ghost_SetVisibilityFlags(ctx, 0x40);
                }
            }
        } else {
        hide:
            if (!Ghost_TestVisibilityFlags(ctx, 0x20)) {
                if ((u8)Ghost_IsHidden(a)) {
                    ctx->elapsed = 0;
                    Ghost_ClearVisibilityFlags(ctx, 0x40);
                    Ghost_SetVisibilityFlags(ctx, 0x20);
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

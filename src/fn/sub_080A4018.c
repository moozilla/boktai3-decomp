#include "scene_actor_chase.h"
// MATCHING_CONCESSION: register-pinning
// confidence=low: periodically probes a randomized heading and updates the Spider route endpoint and cooldown.
#define Spider_UpdateWanderGoal sub_080A4018

#ifdef NONMATCHING
// fuzzy: 80.3%
// RNG/vector allocation, route argument lifetimes, and timer-store scheduling still differ.
struct Transform {
    u8 pad[9];
    u8 facing;
    u8 gap[2];
    struct ContactPosition position;
};
struct SpiderContext {
    u8 pad[0x628];
    s16 timer, delay;
    u16 flags;
};
extern const s16 gUnk_086149C4[];

static inline u8 Test(struct SpiderContext *ctx, u32 mask)
{
    if (ctx->flags & mask)
        return 1;
    return 0;
}

void Spider_UpdateWanderGoal(struct SceneActorRegistryResetView *input)
{
    register struct SceneActorRegistryResetView *a asm("r4") = input;
    register struct SpiderContext *ctx asm("r8") = a->context;
    u8 *group = (u8 *)&a->group;
    register struct Transform *transform asm("r5") = (void *)((u8 *)a + 0x50);
    struct ContactPackedVector target;
    register const u16 *table asm("r6") = gUnk_0203B400;
    register u32 *indexAddress asm("sl") = &gUnk_03005308;
    register u32 mask asm("sb") = 1023;
    u32 index = (*indexAddress + 1) & mask;
    s32 distance = table[index] & mask;
    u8 heading;
    s32 angle;
    distance += 256;
    index = (index + 1) & mask;
    *indexAddress = index;
    angle = (table[index] & 127) - 63;
    heading = angle + transform->facing;
    asm("" : "+r"(heading));
    if (Mod(a->counter2AC, 30) == 0) {
        if (ctx->timer > 0)
            goto decrement;
        target.x = ((gUnk_086149C4[(heading + 64) & 255] * distance) >> 14) +
                   transform->position.x;
        target.y = transform->position.y;
        target.z = ((gUnk_086149C4[heading] * distance) >> 14) + transform->position.z;
        if (!Nav_IsPositionInMesh(*(void **)((u8 *)a + 0x5D0), &a->position, (void *)&target)) {
            if (!Test(ctx, 16))
                a->field2C2 = 64;
            SceneActorMovement_ResolveRoutePosition(a);
            ctx->timer = 0;
        } else {
            *(struct ContactPosition *)((u8 *)a + 0x310) = *(struct ContactPosition *)&target;
            if (!(table[(*indexAddress = (*indexAddress + 1) & mask)] & 3))
                a->field2C2 = 16;
            else
                a->field2C2 = 64;
            {
                void *mesh = *(void **)(group + 0x1FC);
                struct ContactPosition *from = &transform->position;
                struct ContactPosition *to = (void *)((u8 *)a + 0x310);
                u8 moved = Chase_TryRoute(a, mesh, from, to);
                if (!moved) {
                    SceneActorMovement_ResolveRoutePosition(a);
                    ctx->timer = moved;
                }
            }
            if (Test(ctx, 8)) {
                u32 value = 60;
                s16 *timer = &ctx->timer;
                *timer = value;
            } else
                ctx->timer = 120;
        }
    } else if (ctx->timer > 0) {
    decrement: {
        s16 *timer = &ctx->timer;
        u16 value = *timer;
        --value;
        *timer = value;
    }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080A4018);
#endif

#include "scene_actor_chase.h"
// MATCHING_CONCESSION: register-pinning

// confidence=low: periodically probes a randomized heading and updates the Spider route endpoint and cooldown.

struct SpiderMovementView {
    u8 pad[0x310];
    struct ContactPosition goal;
};
struct SpiderGroupView {
    u8 pad[0x1FC];
    void *mesh;
};
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

static inline struct ContactPosition *Spider_GetGoal(u32 offset,
                                                     struct SceneActorRegistryResetView *input)
{
    return (void *)((u8 *)input + offset);
}

static inline u32 RandomChoice(u32 *counter, const u16 *table, u32 mask)
{
    u32 index = (*counter + 1) & mask;
    u32 value;
    *counter = index;
    value = table[index];
    return value & 3;
}
static inline u8 SelectHeading(u16 random, u8 *direction)
{
    s32 angle = (random & 127) - 63;
    return angle + *direction;
}
static inline s32 ReadCounter(struct SceneActorRegistryResetView *actor)
{
    register u32 offset asm("r1") = (u32) & ((struct SceneActorRegistryResetView *)0)->counter2AC;
    s32 *counter = (void *)((u8 *)actor + offset);
    asm("" : : "r"(offset));
    return *counter;
}
void Spider_UpdateWanderGoal(struct SceneActorRegistryResetView *input)
{
    struct SpiderContext *ctx = input->context;
    struct SpiderGroupView *group = (void *)&input->group.common;
    register struct Transform *transform asm("r5") = (void *)&input->body.id;
    struct ContactPackedVector target;
    register const u16 *table asm("r6") = gRandomTable;
    register u32 *indexAddress asm("sl") = &gRandomIndex;
    u32 rawIndex = *indexAddress + 1;
    register u32 mask asm("sb") = 1023;
    u16 index = rawIndex & mask;
    u16 distance = table[index] & mask;
    u8 heading;
    u8 *field;
    distance += 256;
    index = (index + 1) & mask;
    field = (void *)indexAddress;
    *(u32 *)field = index;
    {
        u16 sample = table[index];
        field = &input->body.direction;
        heading = SelectHeading(sample, field);
    }
    if (Mod(ReadCounter(input), 30) == 0) {
        if (ctx->timer > 0)
            goto decrement;
        target.x = ((gUnk_086149C4[(heading + 64) & 255] * distance) >> 14) + transform->position.x;
        target.y = transform->position.y;
        target.z = ((gUnk_086149C4[heading] * distance) >> 14) + transform->position.z;
        if (!Nav_IsPositionInMesh(input->group.route, &input->body.position, (void *)(&target))) {
            if (!Test(ctx, 16))
                input->field2C2 = 64;
            SceneActorMovement_ResolveRoutePosition(input);
            ctx->timer = 0;
        } else {
            ((struct SpiderMovementView *)input)->goal = *((struct ContactPosition *)(&target));
            if (!(RandomChoice(indexAddress, table, mask)))
                input->field2C2 = 16;
            else
                input->field2C2 = 64;
            {
                void *mesh = group->mesh;
                struct ContactPosition *to;
                struct ContactPosition *from = &transform->position;
                u8 moved;
                to = Spider_GetGoal((u32) & ((struct SpiderMovementView *)0)->goal, input);
                moved = Chase_TryRoute(input, mesh, from, to);
                if (!moved) {
                    SceneActorMovement_ResolveRoutePosition(input);
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

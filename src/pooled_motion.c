#include "pooled_motion.h"
#include "scene_actor_components.h"

void MotionPool_FinishOnResourceContact(void *a, void *b, struct MotionPoolSlot *slot)
{
    slot->frame = slot->params.endFrame;
}

void MotionPool_InitializeParams(struct MotionPoolParams *);

void MotionPool_FinishOnResourceContact(void *, void *, struct MotionPoolSlot *);

void Sprite_InitUnlinked(void *, void *, u32);

void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);

static inline void CpuSetSwapped(int mode, const u32 *src, struct ContactPosition *dst)
{
    CpuSet(src, dst, mode);
}

void MotionPool_InitializeSlot(struct MotionPool *pool, struct MotionPoolSlot *slot, s32 index)
{
    struct MotionPoolSlot *slotCopy;
    struct MotionPoolParams *params = &slot->params;
    struct ContactSwarmGraphics *graphics;
    struct ContactCollider *collider;
    u32 zero;
    MotionPool_InitializeParams(params);
    slot->frame = 0;
    {
        struct ContactPosition *velocity = &slot->velocity;
        const u32 *source;
        zero = 0;
        source = &zero;
        CpuSetSwapped(0x05000002, source, velocity);
    }
    graphics = &slot->graphics;
    AnimData_Load(graphics, params->graphicResource);
    Sprite_InitUnlinked(&slot->sprite, graphics, 1);
    slotCopy = slot;
    collider = &slotCopy->collider;
    Collider_Init(collider, 0, 0x2001, 0, 0x10, &params->bounds, &params->offset);
    Collider_SetCallback(collider, MotionPool_FinishOnResourceContact, slot);
    Collider_SetAttackProperties(collider, params->resourceId, params->cost, params->contactMode,
                                 params->contactFlags, params->option);
}

void MotionPool_HideSlot(struct MotionPool *pool, struct MotionPoolSlot *slot, s32 index)
{
    slot->sprite.flags |= 1;
    Sprite_Remove(&slot->sprite);
    pool->activeMask &= ~(1 << index);
}

extern const s16 gUnk_086149C4[];

void MotionPool_HideSlot(struct MotionPool *, struct MotionPoolSlot *, s32);

static inline s32 scaleMotion(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

s32 MotionPool_Update(struct MotionPool *pool)
{
    struct MotionPoolSlot *slot = pool->slots;
    s32 index;
    for (index = 0; index < 16; index++, slot++) {
        if (pool->activeMask & (1 << index)) {
            struct ContactSwarmSprite *sprite = &slot->sprite;
            struct ContactCollider *collider = &slot->collider;
            struct MotionPoolParams *params = &slot->params;
            struct ContactPosition *velocity;
            struct ContactPosition *position;
            if (params->callback(slot, index) == 1) {
                MotionPool_HideSlot(pool, slot, index);
                continue;
            }
            {
                u32 moving = slot->flags & 1;
                velocity = &slot->velocity;
                if (moving) {
                    struct ContactPosition *output = velocity;
                    s32 direction = params->direction;
                    s32 speed = params->speed;
                    output->x = scaleMotion(gUnk_086149C4[(direction + 64) & 255] * speed);
                    output->y = 0;
                    output->z = scaleMotion(gUnk_086149C4[direction & 255] * speed);
                }
            }
            position = &sprite->position;
            sprite->position.x += slot->velocity.x;
            position->y += velocity->y;
            position->z += velocity->z;
            {
                u32 flag = 2;
                if (slot->flags & flag) {
                    Collider_SetPosition(collider, position, 0);
                    Collider_Refresh(collider);
                }
            }
            slot->frame++;
        }
    }
    return 0;
}

void ResourceRegistry_UnregisterResource(u32);

s32 MotionPool_Destroy(struct MotionPool *pool)
{
    struct MotionPoolSlot *slot = pool->slots;
    s32 index = 0;
    do {
        if (pool->activeMask & (1 << index))
            MotionPool_HideSlot(pool, slot, index);
        index++;
        slot++;
    } while (index <= 15);
    ResourceRegistry_UnregisterResource(0x5B1B);
    return 0;
}

void MotionPool_InitializeSlot(struct MotionPool *, struct MotionPoolSlot *, s32);

void ResourceRegistry_RegisterResource(u32, void *, u32);

s32 MotionPool_Initialize(struct MotionPool *pool)
{
    struct MotionPoolSlot *slot;
    s32 index;
    ResourceRegistry_RegisterResource(0x5B1B, pool, 0);
    pool->activeMask = 0;
    slot = pool->slots;
    index = 0;
    do {
        MotionPool_InitializeSlot(pool, slot, index);
        index++;
        slot++;
    } while (index <= 15);
    return 0;
}

struct MotionPool *MotionPool_Create(void)
{
    struct MotionPool *pool = Task_Create(8, sizeof(*pool));
    if (pool) {
        Task_SetCallbacks(pool, (void (*)(void *))MotionPool_Update,
                          (void (*)(void *))MotionPool_Destroy);
        if (MotionPool_Initialize(pool) < 0) {
            Task_Kill(pool);
            return 0;
        }
    }
    return pool;
}

struct MotionPoolSlot *MotionPool_FindFreeSlot(struct MotionPool *pool, s32 *out)
{
    struct MotionPoolSlot *slot = pool->slots;
    s32 index = 0;
    u32 one = 1;
    u32 active = pool->activeMask;
    do {
        if ((one << index & active) != 0) {
            index++;
            slot++;
            continue;
        }
        *out = index;
        return slot;
    } while (index <= 15);
    return 0;
}

struct MotionPool *MotionPool_Create(void);

struct MotionPoolSlot *MotionPool_FindFreeSlot(struct MotionPool *, s32 *);

void *ResourceRegistry_GetResource(u32);

void Sprite_LinkToList(void *, u32);

void *SceneBody_GetById(u32);

s32 MotionPool_Spawn(const struct MotionPoolParams *params, const struct ContactPosition *position)
{
    struct MotionPool *pool = ResourceRegistry_GetResource(0x5B1B);
    struct MotionPoolSlot *slot;
    struct ContactPosition *velocity;
    struct ContactCollider *collider;
    struct ContactSwarmGraphics *graphics;
    struct ContactSwarmSprite *sprite;
    s32 index;
    s32 direction;
    s32 speed;
    if (!pool) {
        MotionPool_Create();
        return -1;
    }
    slot = MotionPool_FindFreeSlot(pool, &index);
    if (!slot)
        return -1;
    Memory_Copy(&slot->params, params, sizeof(*params));
    slot->frame = 0;
    velocity = &slot->velocity;
    direction = params->direction;
    speed = params->speed;
    velocity->x = scaleMotion(gUnk_086149C4[(direction + 64) & 255] * speed);
    velocity->y = 0;
    velocity->z = scaleMotion(gUnk_086149C4[direction & 255] * speed);
    collider = &slot->collider;
    Collider_SetAttackProperties(collider, params->resourceId, params->cost, params->contactMode,
                                 params->contactFlags, params->option);
    collider->bounds = params->bounds;
    collider->offset = params->offset;
    Collider_SetPosition(collider, position, 0);
    slot->sprite.position = *position;
    graphics = &slot->graphics;
    AnimData_Load(graphics, params->graphicResource);
    sprite = &slot->sprite;
    sprite->frame = params->frame;
    if (params->tile >= 0)
        AnimData_SelectPaletteBank(graphics, params->tile);
    sprite->flags &= ~1;
    Sprite_LinkToList(sprite, 0);
    if (params->trackingFrames) {
        struct SceneActorBody *target = SceneBody_GetById(params->targetId);
        if (target)
            slot->target = &target->position;
    }
    pool->activeMask |= 1 << index;
    return 0;
}

void MotionPool_InitializeParams(struct MotionPoolParams *params)
{
    Memory_Clear(params, sizeof(*params));
    MotionPool_SetLifetimeAndVelocity(params, 0, 60, 0, 0);
    MotionPool_SetContact(params, 0, 0, 10, 20, 20);
    MotionPool_SetBounds(params, 8, 32, 8);
    MotionPool_SetOffset(params, 0, 0, 0);
    MotionPool_SetGraphics(params, 0x848F, 0, -1);
    MotionPool_SetCallback(params, MotionPool_UpdateUntilDeadline);
}

void MotionPool_SetLifetimeAndVelocity(struct MotionPoolParams *params, u16 firstFrame,
                                       u16 endFrame, u32 direction, s32 speed)
{
    params->firstFrame = firstFrame;
    params->endFrame = endFrame;
    params->direction = direction;
    params->speed = speed;
}

void MotionPool_SetTracking(struct MotionPoolParams *params, u16 frames, u32 targetId, u8 turnSpeed)
{
    params->trackingFrames = frames;
    params->targetId = targetId;
    params->turnSpeed = turnSpeed;
}

void MotionPool_SetContact(struct MotionPoolParams *params, u32 mode, u32 flags, u32 id, u32 cost,
                           u32 option)
{
    params->contactMode = mode;
    params->contactFlags = flags;
    params->resourceId = id;
    params->cost = cost;
    params->option = option;
}

void MotionPool_SetBounds(struct MotionPoolParams *params, u16 x, u16 y, u16 z)
{
    params->bounds.x = x;
    params->bounds.y = y;
    params->bounds.z = z;
}

void MotionPool_SetOffset(struct MotionPoolParams *params, u16 x, u16 y, u16 z)
{
    params->offset.x = x;
    params->offset.y = y;
    params->offset.z = z;
}

void MotionPool_SetGraphics(struct MotionPoolParams *params, u32 resource, u32 frame, s32 tile)
{
    params->graphicResource = resource;
    params->frame = frame;
    params->tile = tile;
}

void MotionPool_SetCallback(struct MotionPoolParams *params,
                            s32 (*callback)(struct MotionPoolSlot *, s32))
{
    params->callback = callback;
}

void MotionPool_AdjustHeading(struct MotionPoolSlot *slot)
{
    struct MotionPoolParams *params = &slot->params;
    if (slot->frame < params->trackingFrames) {
        const struct ContactPosition *target = slot->target;
        if (target) {
            struct ContactPosition *position = &slot->sprite.position;
            u32 byte = (u8)Math_Atan2(target->x - position->x, target->z - position->z);
            u32 desired;
            u32 current;
            s32 delta;
            u32 step;
            s32 limit;
            u32 flags;
            desired = byte;
            current = (u8)params->direction;
            delta = (s8)desired;
            delta = (s8)(delta - current);
            step = params->turnSpeed;
            limit = (s8)params->turnSpeed;
            if (delta > limit) {
                delta = current + step;
                params->direction = delta;
            } else if (delta < limit) {
                delta = current - step;
                params->direction = delta;
            } else
                params->direction = desired;
            {
                u32 flag = 1;
                flags = slot->flags;
                slot->flags = flags | flag;
            }
        }
    }
}

void MotionPool_AdjustHeading(struct MotionPoolSlot *);

s32 MotionPool_UpdateUntilDeadline(struct MotionPoolSlot *slot, s32 index)
{
    struct MotionPoolParams *params = &slot->params;
    MotionPool_AdjustHeading(slot);
    {
        u32 flag = 2;
        slot->flags |= flag;
    }
    if (slot->frame >= params->endFrame)
        return 1;
    return 0;
}

#include "temporary_animation_pool.h"

s32 TemporaryAnimationPool_Update(struct TemporaryAnimationPool *pool)
{
    struct TemporaryAnimationSlot *slot = pool->slots;
    s32 remaining = 15;
    do {
        if (slot->active) {
            --slot->lifetime;
            if (slot->lifetime <= 0) {
                Anim_Remove(&slot->animation);
                slot->active = 0;
            }
        }
        --remaining;
        ++slot;
    } while (remaining >= 0);
    return 0;
}

s32 TemporaryAnimationPool_Destroy(struct TemporaryAnimationPool *pool)
{
    struct TemporaryAnimationSlot *slot = pool->slots;
    s32 remaining = 15;
    do {
        if (slot->animation.active)
            Anim_Remove(&slot->animation);
        --remaining;
        ++slot;
    } while (remaining >= 0);
    gTemporaryAnimationPool = NULL;
    return 0;
}

s32 Widget_InitCleared(void *, void *, u32, u32, u32, u32, u32, void *);

static inline s32 initializeAnimation(struct SceneInputWidget *animation,
                                      struct SceneInputResource *resource,
                                      struct ContactPosition *position, u32 zero)
{
    s32 value = Widget_InitCleared(animation, resource, 0, 0, 2, zero, 60, position);
    s32 result = 0;
    if (value < 0)
        result = -1;
    return result;
}

s32 TemporaryAnimationPool_Initialize(struct TemporaryAnimationPool *pool)
{
    const struct SceneInputResource *resource;
    struct TemporaryAnimationSlot *slot;
    struct SceneInputResource *relocated;
    struct {
        u32 zero;
        struct ContactPosition position;
    } scratch;
    s32 index;
    s32 result;
    u32 fillZero;
    struct ContactPosition *point;
    struct SceneInputResource *savedResource;
    gTemporaryAnimationPool = pool;
    resource = ScriptProc_Start(0xCB05, 0xDE23);
    if (!resource) {
        result = -1;
        goto done;
    }
    pool->resource = *resource;
    relocated = &pool->resource;
    ResourceHeader_Relocate(relocated, (void *)resource);
    slot = pool->slots;
    index = 0;
    savedResource = relocated;
    point = &scratch.position;
    fillZero = 0;
    do {
        scratch.zero = fillZero;
        CpuSet(&scratch.zero, point, 0x05000002);
        if (initializeAnimation(&slot->animation, savedResource, point, fillZero) < 0) {
            result = -1;
            goto done;
        }
        ++index;
        ++slot;
    } while (index <= 15);
    result = 0;
done:
    return result;
}

s32 TemporaryAnimationPool_Update(struct TemporaryAnimationPool *);

s32 TemporaryAnimationPool_Destroy(struct TemporaryAnimationPool *);

s32 TemporaryAnimationPool_Initialize(struct TemporaryAnimationPool *);

struct TemporaryAnimationPool *TemporaryAnimationPool_Create(void)
{
    struct TemporaryAnimationPool *pool = gTemporaryAnimationPool;
    if (!pool) {
        pool = Task_Create(10, sizeof(*pool));
        if (pool) {
            Task_SetCallbacks(pool, (void (*)(void *))TemporaryAnimationPool_Update,
                              (void (*)(void *))TemporaryAnimationPool_Destroy);
            if (TemporaryAnimationPool_Initialize(pool) < 0) {
                Task_Kill(pool);
                return NULL;
            }
        }
    }
    return pool;
}

struct TemporaryAnimationSlot *
TemporaryAnimationPool_FindInactive(struct TemporaryAnimationPool *pool)
{
    struct TemporaryAnimationSlot *slot = pool->slots;
    s32 index = 0;
    while (index <= 15) {
        if (!slot->active)
            return slot;
        ++index;
        ++slot;
    }
    return NULL;
}

struct TemporaryAnimationPool *TemporaryAnimationPool_Create(void);

struct TemporaryAnimationSlot *TemporaryAnimationPool_FindInactive(struct TemporaryAnimationPool *);

void Widget_CopyMasked(void *, u32, u32);

void Anim_LinkToList(void *, u32);

struct TemporaryAnimationSlot *TemporaryAnimationPool_CreateEntry(u32 sequence, u32 lifetime,
                                                                  u32 flags, s32 paletteIndex,
                                                                  const void *paletteData)
{
    struct TemporaryAnimationPool *pool = gTemporaryAnimationPool;
    struct TemporaryAnimationSlot *slot;
    struct SceneInputWidget *animation;
    if (!pool) {
        pool = TemporaryAnimationPool_Create();
        if (!pool)
            return NULL;
    }
    slot = TemporaryAnimationPool_FindInactive(pool);
    if (!slot)
        return NULL;
    animation = &slot->animation;
    Widget_CopyMasked(animation, sequence, 27);
    animation->flags |= flags;
    if (paletteIndex > 0) {
        animation->paletteBank = paletteIndex;
        if (paletteData)
            animation->palette = (u16 *)paletteData;
        else
            animation->palette = (u16 *)(gPaletteBankData + paletteIndex * 32);
    }
    if (animation->flags & 128)
        Anim_LinkToList(animation, 1);
    else
        Anim_LinkToList(animation, 0);
    slot->lifetime = lifetime;
    slot->active = 1;
    return slot;
}

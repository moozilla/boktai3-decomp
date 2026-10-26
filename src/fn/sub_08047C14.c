#include "temporary_animation_pool.h"
s32 Widget_InitCleared(void *, void *, u32, u32, u32, u32, u32, void *);
static inline s32 initializeAnimation(struct ProjectedDisplayEntry *animation,
                                      struct ProjectedDisplayResource *resource,
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
    const struct ProjectedDisplayResource *resource;
    struct TemporaryAnimationSlot *slot;
    struct ProjectedDisplayResource *relocated;
    struct {
        u32 zero;
        struct ContactPosition position;
    } scratch;
    s32 index;
    s32 result;
    u32 fillZero;
    struct ContactPosition *point;
    struct ProjectedDisplayResource *savedResource;
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

// MATCHING_CONCESSION: register-pinning
#include "temporary_animation_pool.h"
s32 sub_08219918(void *, void *, u32, u32, u32, u32, u32, void *);
static inline s32 initializeAnimation(struct ProjectedDisplayEntry *animation,
                                      struct ProjectedDisplayResource *resource,
                                      struct ContactPosition *position, u32 zero)
{
    s32 value = sub_08219918(animation, resource, 0, 0, 2, zero, 60, position);
    s32 result = 0;
    if (value < 0)
        result = -1;
    return result;
}
s32 TemporaryAnimationPool_Initialize(struct TemporaryAnimationPool *pool)
{
    const struct ProjectedDisplayResource *resource;
    register struct TemporaryAnimationSlot *slot asm("r5");
    struct ProjectedDisplayResource *relocated;
    struct {
        u32 zero;
        struct ContactPosition position;
    } scratch;
    register s32 index asm("r6");
    s32 result;
    u32 fillZero;
    struct ContactPosition *point;
    struct ProjectedDisplayResource *savedResource;
    gUnk_020000A8 = pool;
    resource = ScriptProc_Start(0xCB05, 0xDE23);
    if (!resource) {
        result = -1;
        goto done;
    }
    {
        struct ProjectedDisplayResource *destination = &pool->resource;
        const struct ProjectedDisplayResource *source = resource;
        *destination = *source;
    }
    asm("" : "+r"(pool));
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

// MATCHING_CONCESSION: register-pinning
#include "temporary_animation_pool.h"
#define TemporaryAnimationPool_CreateEntry sub_08047D24
#define TemporaryAnimationPool_Create sub_08047CB8
#define TemporaryAnimationPool_FindInactive sub_08047D04
struct TemporaryAnimationPool *TemporaryAnimationPool_Create(void);
struct TemporaryAnimationSlot *TemporaryAnimationPool_FindInactive(struct TemporaryAnimationPool *);
void sub_082199C8(void *, u32, u32);
void sub_08214424(void *, u32);
struct TemporaryAnimationSlot *TemporaryAnimationPool_CreateEntry(u32 sequence, u32 lifetime, u32 flags, s32 paletteIndex, const void *paletteData)
{
    struct TemporaryAnimationPool *pool = gUnk_020000A8;
    struct TemporaryAnimationSlot *slot;
    struct ProjectedDisplayEntry *animation;
    if (!pool) {
        pool = TemporaryAnimationPool_Create();
        if (!pool)
            return NULL;
    }
    slot = TemporaryAnimationPool_FindInactive(pool);
    if (!slot)
        return NULL;
    animation = &slot->animation;
    sub_082199C8(animation, sequence, 27);
    animation->flags |= flags;
    if (paletteIndex > 0) {
        const void *palette;
        animation->paletteIndex = paletteIndex;
        palette = paletteData;
        if (!palette)
            palette = gUnk_030042E4 + paletteIndex * 32;
        animation->paletteData = palette;
    }
    if (animation->flags & 128)
        sub_08214424(animation, 1);
    else
        sub_08214424(animation, 0);
    {
        register u32 frames asm("r1") = lifetime;
        slot->lifetime = frames;
    }
    slot->active = 1;
    return slot;
}

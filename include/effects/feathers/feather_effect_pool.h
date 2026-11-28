#ifndef GUARD_FEATHER_EFFECT_POOL_H
#define GUARD_FEATHER_EFFECT_POOL_H

#include "engine/task.h"
#include "scene/collision/contact_position.h"
#include "engine/video/display_sequence.h"

struct FeatherSprite {
    u32 flags;
    u8 linked;
    u8 unknown5[2], priority;
    u8 unknown8[4];
    void *animData;
    u16 frame;
    u8 unknown12[0xA];
    struct ContactPosition position;
    u8 unknown24[8];
};

/* Tentative feather identity. The allocator and destructor traverse twenty
 * 0xA0-byte entries; the complete task allocation is 0xCBC bytes. */
struct Feather {
    struct FeatherSprite sprite;
    struct DisplaySequenceState sequence;
    u8 unknown3C[0x54];
    u16 age;
    u8 state;
    s8 slot;
    u16 blinkAge;
    u8 enter, angle;
    u32 effectFlags, variant;
};

struct FeatherPool {
    struct Task task;
    u8 graphics[0x1C];
    void *sequences;
    u32 used;
    struct Feather slots[20];
};

extern u32 gUnk_02000560;

/* The scene lock bits that hold feather effects back. */
static inline u32 Feather_LockBits(u32 mask)
{
    return gUnk_02000560 & mask;
}

static inline u8 FeatherPool_IsUsed(const struct FeatherPool *pool, s32 slot)
{
    if (pool->used & (1 << slot))
        return TRUE;
    return FALSE;
}

/* Facing row and horizontal flip of a feather travelling at angle. */
static inline void Feather_SelectDirection(u16 angle, u8 *row, u8 *flip)
{
    u32 quadrant = (((angle & 255) + 0x60) >> 6) & 3;

    if (quadrant > 1) {
        *flip = 1;
        *row = 3 - quadrant;
    } else {
        *flip = 0;
        *row = quadrant;
    }
}

typedef void (*FeatherStateCallback)(struct FeatherPool *, struct Feather *);
extern FeatherStateCallback const gUnk_086107A0[4];
extern struct FeatherPool *gFadingSpritePool;

void sub_08128B48(struct FeatherPool *, struct Feather *);
struct Feather *FeatherEffectPool_AllocEntry(struct FeatherPool *);
void FeatherEffectPool_UpdateOpeningAnimation(struct FeatherPool *, struct Feather *);
u8 FeatherEffectPool_ClipScatterPath(struct Feather *, struct ContactPosition *, s32, s32, s32);
void FeatherEffectPool_SpawnGroundScatter(struct Feather *, s32);
void FeatherEffectPool_UpdateScatterAnimation(struct FeatherPool *, struct Feather *);
void FadingSprite_UpdateExpiry(struct FeatherPool *, struct Feather *);
u32 FeatherEffectPool_UpdateTask(struct FeatherPool *);
u32 FeatherEffectPool_DestroyTask(struct FeatherPool *);
s32 FeatherEffectPool_Init(struct FeatherPool *);
struct FeatherPool *FeatherEffectPool_GetOrCreate(void);

#endif

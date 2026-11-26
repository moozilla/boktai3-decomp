#ifndef GUARD_FEATHER_EFFECT_POOL_H
#define GUARD_FEATHER_EFFECT_POOL_H

#include "engine/task.h"
#include "effects/projectiles/contact_swarm_sprite.h"
#include "engine/video/display_sequence.h"

/* Tentative feather identity. The allocator and destructor traverse twenty
 * 0xA0-byte entries; the complete task allocation is 0xCBC bytes. */
struct Feather {
    struct ContactSwarmSprite sprite;
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

typedef void (*FeatherStateCallback)(struct FeatherPool *, struct Feather *);
extern FeatherStateCallback const gUnk_086107A0[4];
extern struct FeatherPool *gFadingSpritePool;

void sub_08128B48(struct FeatherPool *, struct Feather *);
void FeatherEffectPool_UpdateOpeningAnimation(struct FeatherPool *, struct Feather *);
void FeatherEffectPool_UpdateScatterAnimation(struct FeatherPool *, struct Feather *);
void FadingSprite_UpdateExpiry(struct FeatherPool *, struct Feather *);
u32 FeatherEffectPool_UpdateTask(struct FeatherPool *);
struct FeatherPool *FeatherEffectPool_GetOrCreate(void);

#endif

#ifndef GUARD_FEATHER_EFFECT_POOL_H
#define GUARD_FEATHER_EFFECT_POOL_H

#include "task.h"
#include "contact_position.h"
#include "display_sequence.h"

/* Tentative feather identity. The allocator and destructor traverse twenty
 * 0xA0-byte entries; the complete task allocation is 0xCBC bytes. */
struct Feather {
    u32 flags;
    u8 unknown4[3], priority;
    u8 unknown8[4];
    void *graphics;
    u16 frame;
    u8 unknown12[10];
    struct ContactPosition position;
    u8 unknown24[8];
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
extern FeatherStateCallback const gUnk_086107A0[];
extern struct FeatherPool *gFadingSpritePool;

u32 FeatherEffectPool_UpdateTask(struct FeatherPool *);

#endif

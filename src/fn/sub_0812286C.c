#include "global.h"
#include "ram.h"

// foundry draft (worker best 87.3%); what differs is mostly register allocation and instruction order
#define TerrainOverlay_QueueRemoveBody TerrainOverlay_QueueRemoveActor
#define SceneBody_Release              SceneBody_Unregister

struct AnimatedBodySlot {
    u8 animation[0x60];
    u32 body[0x38];
    u16 flags;
    u16 unk142;
};
struct AnimatedBodyFlagsView {
    u16 flags;
    u8 unk02[0x142];
};
struct AnimatedBodyPool {
    u8 unk00[0x3c];
    struct AnimatedBodySlot slots[4];
    u32 occupied;
};
void Anim_Remove(void *);
u32 TerrainOverlay_QueueRemoveActor(u32 *);
void SceneBody_Unregister(void *);
static inline u8 HasSlot(u32 *occupied, u32 bit)
{
    if (*occupied & bit)
        return 1;
    return 0;
}

s32 AnimatedBodyPool_Destroy(struct AnimatedBodyPool *pool)
{
    s32 i = 0;
    u32 *occupied = &pool->occupied;
    struct AnimatedBodyFlagsView *flags;
    struct AnimatedBodySlot *slot;
    flags = (void *)&pool->slots[0].flags;
    slot = pool->slots;
    do {
        struct AnimatedBodySlot *bodies = (void *)pool->slots[0].body;
        if (HasSlot(occupied, 1 << i)) {
            Anim_Remove(&slot[i]);
            if (flags[i].flags & 1)
                TerrainOverlay_QueueRemoveActor((u32 *)&bodies[i]);
            SceneBody_Unregister(slot[i].body);
        }
        i++;
    } while (i <= 3);
    gAnimatedBodyPool = 0;
    return 0;
}

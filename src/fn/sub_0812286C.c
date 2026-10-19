
#ifdef NONMATCHING
// fuzzy: 87.3%
// foundry draft (worker best 87.3%); what differs is mostly register allocation and instruction order
#define AnimatedBodyPool_Destroy sub_0812286C
#define TerrainOverlay_QueueRemoveBody TerrainOverlay_QueueRemoveActor
#define SceneBody_Release SceneBody_Unregister
#define gAnimatedBodyPool gUnk_02000140
#include "global.h"

struct AnimatedBodySlot {
    u8 animation[0x60];
    u32 body[0x38];
    u16 flags;
    u16 unk142;
};
struct AnimatedBodyFlagsView { u16 flags; u8 unk02[0x142]; };
struct AnimatedBodyPool {
    u8 unk00[0x3c];
    struct AnimatedBodySlot slots[4];
    u32 occupied;
};
extern struct AnimatedBodyPool *gUnk_02000140;
void Anim_Remove(void *);
u32 TerrainOverlay_QueueRemoveActor(u32 *);
void SceneBody_Unregister(void *);
static inline u8 HasSlot(u32 *occupied, u32 bit)
{
    if (*occupied & bit)
        return 1;
    return 0;
}

s32 sub_0812286C(struct AnimatedBodyPool *pool)
{
    s32 i = 0;
    u32 *occupied = &pool->occupied;
    struct AnimatedBodySlot *slot = pool->slots;
    struct AnimatedBodySlot *bodies = (void *)pool->slots[0].body;
    struct AnimatedBodyFlagsView *flags = (void *)&pool->slots[0].flags;
    do {
        if (HasSlot(occupied, 1 << i)) {
            Anim_Remove(slot);
            if (flags[i].flags & 1)
                TerrainOverlay_QueueRemoveActor((u32 *)&bodies[i]);
            SceneBody_Unregister(slot->body);
        }
        slot++;
        i++;
    } while (i <= 3);
    gUnk_02000140 = 0;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0812286C);
#endif

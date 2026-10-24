#include "global.h"
struct ScenePartner {
    u8 pad0[0x20C];
    s16 health;
    u8 pad20E[0x21C - 0x20E];
};
struct PairedSceneActor {
    struct ScenePartner partners[2];
};
static inline u8 IsActive(struct PairedSceneActor *p, s32 i)
{
    u8 active = 0;
    if (p->partners[i].health > 0)
        active = 1;
    return active;
}
// confidence=low: how many of the two partners have a positive counter.
u32 ScenePair_CountPositiveCounters(struct PairedSceneActor *p)
{
    u32 count = 0;
    if (IsActive(p, 0))
        count++;
    if (IsActive(p, 1))
        count++;
    return count;
}

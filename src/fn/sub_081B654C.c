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
// confidence=low: bit 0 when the first partner's counter is positive, bit 1 for the second.
u32 PairedSceneActor_GetActiveMask(struct PairedSceneActor *p)
{
    u32 mask = 0;
    if (IsActive(p, 0))
        mask |= 1;
    if (IsActive(p, 1))
        mask |= 2;
    return mask;
}

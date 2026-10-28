#include "global.h"
#include "engine_globals.h"
#include "ram.h"
struct PairedSceneActor;
s32 PairedSceneActor_GetActiveMask(struct PairedSceneActor *);
s32 PairedSceneActor_ApplyToSelectedHalves(struct PairedSceneActor *, u32, u32);
static inline s32 NextRandom(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(u16 *)(gRandomIndex * 2 + (u32)gRandomTable);
}
s32 PairedSceneActor_ApplyToActiveHalf(struct PairedSceneActor *p, u32 value)
{
    switch (PairedSceneActor_GetActiveMask(p)) {
        case 0:
            return 0;
        case 2:
            return PairedSceneActor_ApplyToSelectedHalves(p, 2, value);
        default:
            if (!((NextRandom() >> 3) & 1))
                return PairedSceneActor_ApplyToSelectedHalves(p, 2, value);
        case 1:
            return PairedSceneActor_ApplyToSelectedHalves(p, 1, value);
    }
}

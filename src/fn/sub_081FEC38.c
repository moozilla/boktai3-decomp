#include "global.h"
#include "sun_orb_effect.h"
static inline void DisableOrbCollider(struct ContactCollider *state)
{
    state->flags |= 4;
}
s32 sub_081FEC38(struct OrbitEffectManager *manager, s32 index)
{
    if (manager->activeSlots & (1 << index)) {
        DisableOrbCollider(&manager->slots[index].collider);
        SunOrb_SetUpdateCallback(&manager->slots[index].params, OrbitEffect_UpdateContraction);
        return 0;
    }
    return -1;
}

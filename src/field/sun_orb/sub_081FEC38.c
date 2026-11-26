#include "engine/shared/global.h"
#include "field/sun_orb/sun_orb_effect.h"
#include "bosses/vanargand_projectile/vanargand_projectiles.h"
s32 sub_081FEC38(struct OrbitEffectManager *manager, s32 index)
{
    if (manager->activeSlots & (1 << index)) {
        EffectCollision_Disable(&manager->slots[index].collider);
        SunOrb_SetUpdateCallback(&manager->slots[index].params, OrbitEffect_UpdateContraction);
        return 0;
    }
    return -1;
}

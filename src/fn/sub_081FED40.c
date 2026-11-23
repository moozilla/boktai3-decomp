#include "global.h"
#include "sun_orb_effect.h"
#include "ram.h"
#include "vanargand_projectiles.h"
void Sound_StopSong(u32);
s32 sub_081FED40(s32 index)
{
    struct OrbitEffectManager *manager = (struct OrbitEffectManager *)gSlotController;
    if (!manager || !(manager->activeSlots & (1 << index)))
        return -1;
    EffectCollision_Disable(&manager->slots[index].collider);
    SunOrb_SetUpdateCallback(&manager->slots[index].params, OrbitEffect_UpdateContraction);
    Sound_StopSong(0x564);
    return 0;
}

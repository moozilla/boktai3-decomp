#include "bosses/vanargand_projectile/vanargand_projectiles.h"
#define RisingActor_SetCallback SunOrb_SetUpdateCallback
#include "engine/shared/global.h"
#include "field/sun_orb/sun_orb_effect.h"

s32 Mod(s32, s32);
s32 RisingActor_LowerHeight(struct OrbitEffect *actor, s32 slot)
{
    if (Mod(actor->frame, 4) == 0)
        actor->scale += -0x2000;
    if (actor->scale <= 0x40000) {
        actor->volley.lowered = 1;
        EffectCollision_Disable(&actor->collider);
        SunOrb_SetUpdateCallback(&actor->params, OrbitEffect_FireVolley);
    }
    return 0;
}

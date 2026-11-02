#include "global.h"
#include "sun_orb_effect.h"
void Memory_Clear(void *, u32);
void RisingActorConfig_Initialize(struct OrbitEffectParams *config)
{
    Memory_Clear(config, sizeof(*config));
    SunOrb_SetUpdateCallback(config, OrbitEffect_FireVolley);
    config->unk_00 = 0;
    config->lifetime = 600;
    config->speed = 4;
    config->angle = 0;
    config->unk_0C = 2;
    config->unk_0D = 0;
    config->collisionX = 30;
    config->collisionY = 15;
    config->collisionZ = 30;
    config->kind = 1;
    config->homing = 0;
}
